import sqlite3
from contextlib import closing
from uuid import UUID

from models import Client, Message


# Create SQL tables if thy do not exist
def initialize_db(db_path: str) -> None:
    conn = sqlite3.connect(db_path)
    conn.executescript('''
    CREATE TABLE IF NOT EXISTS clients (
        ID BINARY(16) PRIMARY KEY,
        UserName VARCHAR(255) NOT NULL UNIQUE,
        PublicKey BINARY(160) NOT NULL,
        LastSeen DATETIME NOT NULL);
    CREATE TABLE IF NOT EXISTS messages (
        ID INTEGER PRIMARY KEY AUTOINCREMENT,
        ToClient BINARY(16) NOT NULL,
        FromClient BINARY(16) NOT NULL,
        Type INTEGER NOT NULL,
        Content BLOB NOT NULL,
        FOREIGN KEY (ToClient) REFERENCES clients(ID),
        FOREIGN KEY (FromClient) REFERENCES clients(ID));
    ''')
    conn.commit()
    conn.close()


# Commands to select or save data to the database.
class DB:
    def __init__(self, db_path: str) -> None:
        self.conn = sqlite3.connect(db_path)

    def close(self) -> None:
        self.conn.close()

    def get_client(self, client_id: UUID) -> Client | None:
        with closing(self.conn.cursor()) as curr:
            curr.execute('SELECT * FROM clients WHERE ID = ?', (client_id.bytes, ))
            row = curr.fetchone()
        if row is None:
            return None
        return Client(UUID(bytes=row[0]), row[1], row[2], row[3])

    def get_client_by_username(self, username: bytes) -> Client | None:
        with closing(self.conn.cursor()) as curr:
            curr.execute('SELECT * FROM clients WHERE UserName = ?', (username, ))
            row = curr.fetchone()
        if row is None:
            return None
        return Client(UUID(bytes=row[0]), row[1], row[2], row[3])

    def add_client(self, client: Client) -> None:
        with closing(self.conn.cursor()) as curr:
            curr.execute('INSERT INTO clients (ID, UserName, PublicKey, LastSeen) VALUES (?, ?, ?, ?)',
                         (client.client_id.bytes, client.username, client.public_key, client.last_seen))
            self.conn.commit()

    def update_client_last_seen(self, client_id: UUID) -> None:
        with closing(self.conn.cursor()) as curr:
            curr.execute('UPDATE clients SET LastSeen = CURRENT_TIMESTAMP WHERE ID = ?', (client_id.bytes, ))
            self.conn.commit()

    def get_all_clients(self, exclude: UUID) -> list[Client]:
        with closing(self.conn.cursor()) as curr:
            curr.execute('SELECT * FROM clients WHERE ID != ?', (exclude.bytes, ))
            rows = curr.fetchall()
        return [Client(UUID(bytes=row[0]), row[1], row[2], row[3]) for row in rows]

    def add_message(self, msg: Message) -> int:
        with closing(self.conn.cursor()) as curr:
            curr.execute('INSERT INTO messages (ToClient, FromClient, Type, Content) VALUES (?, ?, ?, ?)',
                         (msg.to_client.bytes, msg.from_client.bytes, msg.type, msg.content))
            # Will only select rows by the current connection, so will return correct ID
            curr.execute('SELECT last_insert_rowid()')
            msg_id = curr.fetchone()[0]
            self.conn.commit()
        return msg_id

    def get_messages_for_client(self, client_id: UUID) -> list[Message]:
        with closing(self.conn.cursor()) as curr:
            curr.execute('SELECT * FROM messages WHERE ToClient = ?', (client_id.bytes, ))
            rows = curr.fetchall()
        return [Message(row[0], UUID(bytes=row[1]), UUID(bytes=row[2]), row[3], row[4]) for row in rows]

    def delete_messages(self, message_ids: list[int]) -> None:
        with closing(self.conn.cursor()) as curr:
            curr.execute(f'DELETE FROM messages WHERE ID IN ({", ".join(['?'] * len(message_ids))})', message_ids)
            self.conn.commit()
