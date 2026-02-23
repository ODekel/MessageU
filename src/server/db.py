import sqlite3
from contextlib import closing
from uuid import UUID

from models import Client, Message

_conn: sqlite3.Connection | None = None


def initialize_db(db_path: str) -> None:
    conn = sqlite3.connect(db_path)
    conn.executescript('''
    CREATE TABLE IF NOT EXISTS clients (
        ID BINARY(16) PRIMARY KEY,
        UserName VARCHAR(255) NOT NULL,
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
    global _conn
    _conn = conn


def close_db() -> None:
    global _conn
    if _conn is not None:
        _conn.close()
        _conn = None


def get_client(client_id: UUID) -> Client | None:
    with closing(_conn.cursor()) as curr:
        curr.execute('SELECT * FROM clients WHERE ID = ?', (client_id.bytes, ))
        row = curr.fetchone()
        if row is None:
            return None
        return Client(UUID(bytes=row[0]), row[1], row[2], row[3])


def add_client(client: Client) -> None:
    with closing(_conn.cursor()) as curr:
        curr.execute('INSERT INTO clients (ID, UserName, PublicKey, LastSeen) VALUES (?, ?, ?, ?)',
                     (client.client_id.bytes, client.username, client.public_key, client.last_seen))
        _conn.commit()


def update_client_last_seen(client_id: UUID) -> None:
    with closing(_conn.cursor()) as curr:
        curr.execute('UPDATE clients SET LastSeen = CURRENT_TIMESTAMP WHERE ID = ?', (client_id.bytes, ))
        _conn.commit()


def get_all_clients(exclude: UUID) -> list[Client]:
    with closing(_conn.cursor()) as curr:
        curr.execute('SELECT * FROM clients WHERE ID != ?', (exclude.bytes, ))
        rows = curr.fetchall()
        return [Client(UUID(bytes=row[0]), row[1], row[2], row[3]) for row in rows]


def add_message(msg: Message) -> None:
    with closing(_conn.cursor()) as curr:
        curr.execute('INSERT INTO messages (ToClient, FromClient, Type, Content) VALUES (?, ?, ?, ?)',
                     (msg.to_client.bytes, msg.from_client.bytes, msg.type, msg.content))
        _conn.commit()


def get_messages_for_client(client_id: UUID) -> list[Message]:
    with closing(_conn.cursor()) as curr:
        curr.execute('SELECT * FROM messages WHERE ToClient = ?', (client_id.bytes, ))
        rows = curr.fetchall()
        return [Message(row[0], UUID(bytes=row[1]), UUID(bytes=row[2]), row[3], row[4]) for row in rows]


def delete_messages(message_ids: list[int]) -> None:
    with closing(_conn.cursor()) as curr:
        curr.execute(f'DELETE FROM messages WHERE ID IN ({", ".join(['?'] * len(message_ids))})',
                     message_ids)
        _conn.commit()
