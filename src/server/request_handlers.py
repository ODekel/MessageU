import datetime
import struct
import uuid
from collections.abc import Buffer

from db import DB
from models import RequestHeaders, ServerException, Client, Message


_USERNAME_MAX_LENGTH = 255


def _wrapped_unpack(fmt: str | bytes, buffer: Buffer) -> tuple:
    try:
        return struct.unpack(fmt, buffer)
    except struct.error:
        raise ServerException('Invalid request format.')


def _extract_username(username_bytes: bytes) -> bytes:
    parts = username_bytes.split(b'\0', 1)
    if len(parts) == 1:
        raise ServerException('Invalid username format.')
    return parts[0]


def _validate_client_registered(headers: RequestHeaders, db: DB) -> None:
    if db.get_client(headers.client_id) is None:
        raise ServerException('Client not registered.')


def register_client(_: RequestHeaders, content: bytes, db: DB) -> bytes:
    raw_username, public_key = _wrapped_unpack(f'!{_USERNAME_MAX_LENGTH}s160s', content)
    username = _extract_username(raw_username)
    if db.get_client_by_username(username) is not None:
        raise ServerException(f'Client with username "{username}" already exists.')
    client_id = uuid.uuid4()
    db.add_client(Client(client_id, username, public_key, datetime.datetime.now(datetime.UTC)))
    return client_id.bytes


def get_client_list(headers: RequestHeaders, _: bytes, db: DB) -> bytes:
    _validate_client_registered(headers, db)
    clients = db.get_all_clients(headers.client_id)
    db.update_client_last_seen(headers.client_id)
    result = b''
    for client in clients:
        result += struct.pack(f'!16s{_USERNAME_MAX_LENGTH}s',
                              client.client_id.bytes, client.username.ljust(_USERNAME_MAX_LENGTH, b'\0'))
    return result


def get_client_public_key(headers: RequestHeaders, content: bytes, db: DB) -> bytes:
    _validate_client_registered(headers, db)
    client_id = uuid.UUID(bytes=_wrapped_unpack('!16s', content)[0])
    client = db.get_client(client_id)
    db.update_client_last_seen(headers.client_id)
    if client is None:
        raise ServerException(f'Client with ID {client_id} not found.')
    return client_id.bytes + client.public_key


def send_message(headers: RequestHeaders, content: bytes, db: DB) -> bytes:
    _validate_client_registered(headers, db)
    to_id, msg_type, msg_content_size = _wrapped_unpack('!16sBI', content[:21])
    msg_context = content[21:]
    msg_id = db.add_message(Message(0, uuid.UUID(bytes=to_id), headers.client_id, msg_type, msg_context))
    db.update_client_last_seen(headers.client_id)
    return struct.pack('!16sI', to_id, msg_id)


def get_messages(headers: RequestHeaders, _: bytes, db: DB) -> bytes:
    _validate_client_registered(headers, db)
    messages = db.get_messages_for_client(headers.client_id)
    db.update_client_last_seen(headers.client_id)
    db.delete_messages([msg.message_id for msg in messages])
    result = b''
    for msg in messages:
        result += (struct.pack('!16sIBI', msg.from_client.bytes, msg.message_id, msg.type, len(msg.content))
                   + msg.content)
    return result
