import socket
import struct
from collections.abc import Callable
from uuid import UUID

from models import RequestHeaders, ServerException
from request_handlers import register_client, get_client_list, get_client_public_key, send_message, get_messages

_CODE_MAPPING: dict[int, Callable[[RequestHeaders, bytes], bytes]] = {
    700: register_client,
    701: get_client_list,
    702: get_client_public_key,
    703: send_message,
    704: get_messages
}


def _unknown_code(headers: RequestHeaders, _: bytes):
    raise ServerException(f'got unknown code: {headers.code}')


def handler(conn: socket.socket) -> bytes:
    raw_headers = conn.recv(23)
    client_id, version, code, size = struct.unpack('!16sBhI', raw_headers)
    headers = RequestHeaders(UUID(bytes=client_id), version, code, size)
    if headers.size > 0:
        content = conn.recv(headers.size)
    else:
        content = b''
    return _CODE_MAPPING.get(headers.code, _unknown_code)(headers, content)
