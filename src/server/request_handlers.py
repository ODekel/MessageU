import datetime
import struct
import uuid

import db
from models import RequestHeaders, ServerException, Client, Message


def register_client(_: RequestHeaders, content: bytes) -> bytes:
    username, public_key = struct.unpack('!255s160s', content)
    if db.get_client_by_username(username) is not None:
        raise ServerException(f'Client with username "{username}" already exists.')
    client_id = uuid.uuid4()
    db.add_client(Client(client_id, username, public_key, datetime.datetime.now(datetime.UTC)))
    return client_id.bytes


def get_client_list(headers: RequestHeaders, _: bytes) -> bytes:
    clients = db.get_all_clients(headers.client_id)
    db.update_client_last_seen(headers.client_id)
    result = b''
    for client in clients:
        result += struct.pack('!16s255s', client.client_id.bytes, client.username)
    return result


def get_client_public_key(headers: RequestHeaders, content: bytes) -> bytes:
    client_id = uuid.UUID(bytes=content)
    client = db.get_client(client_id)
    db.update_client_last_seen(headers.client_id)
    if client is None:
        raise ServerException(f'Client with ID {client_id} not found.')
    return client_id.bytes + client.public_key


def send_message(headers: RequestHeaders, content: bytes) -> bytes:
    to_id, msg_type, msg_content_size = struct.unpack('!16sBI', content[:21])
    msg_context = content[21:]
    msg_id = db.add_message(Message(0, uuid.UUID(bytes=to_id), headers.client_id, msg_type, msg_context))
    db.update_client_last_seen(headers.client_id)
    return struct.pack('!16sI', to_id, msg_id)


def get_messages(headers: RequestHeaders, _: bytes) -> bytes:
    messages = db.get_messages_for_client(headers.client_id)
    db.update_client_last_seen(headers.client_id)
    db.delete_messages([msg.message_id for msg in messages])
    result = b''
    for msg in messages:
        result += (struct.pack('!16sIBI', msg.from_client.bytes, msg.message_id, msg.type, len(msg.content))
                   + msg.content)
    return result
