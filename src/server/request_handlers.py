import datetime
import struct
import uuid

import db
from models import RequestHeaders, ServerException, Client


def register_client(headers: RequestHeaders, content: bytes) -> bytes:
    username, public_key = struct.unpack('!255s160s', content)
    if db.get_client_by_username(username) is not None:
        raise ServerException(f'Client with username "{username}" already exists.')
    client_id = uuid.uuid4()
    db.add_client(Client(client_id, username, public_key, datetime.datetime.now(datetime.UTC)))
    return client_id.bytes


def get_client_list(headers: RequestHeaders, content: bytes) -> bytes:
    pass


def get_client_public_key(headers: RequestHeaders, content: bytes) -> bytes:
    pass


def send_message(headers: RequestHeaders, content: bytes) -> bytes:
    pass


def get_messages(headers: RequestHeaders, content: bytes) -> bytes:
    pass
