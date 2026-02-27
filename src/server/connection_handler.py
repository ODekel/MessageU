import socket
import struct
from collections.abc import Callable
from uuid import UUID

from db import DB
from models import RequestHeaders, ServerException
from request_handlers import register_client, get_client_list, get_client_public_key, send_message, get_messages


_CODE_TO_HANDLER_MAPPING: dict[int, Callable[[RequestHeaders, bytes, DB], bytes]] = {
    700: register_client,
    701: get_client_list,
    702: get_client_public_key,
    703: send_message,
    704: get_messages
}

_CODE_TO_RES_CODE_MAPPING: dict[int, int] = {
    700: 2100,
    701: 2101,
    702: 2102,
    703: 2103,
    704: 2104
}

_ERROR_RES_CODE = 9000


def _unknown_code(headers: RequestHeaders, _: bytes):
    raise ServerException(f'unknown code: {headers.code}')


def _get_response(headers: RequestHeaders, content: bytes, db: DB, server_version: int) -> bytes:
    try:
        res_content = _CODE_TO_HANDLER_MAPPING.get(headers.code, _unknown_code)(headers, content, db)
        res_code = _CODE_TO_RES_CODE_MAPPING[headers.code]
    except ServerException:
        res_content = b''
        res_code = _ERROR_RES_CODE
    return struct.pack('!BHI', server_version, res_code, len(res_content)) + res_content


def handler(sock: socket.socket, db_factory: Callable[[], DB], server_version: int) -> None:
    db = db_factory()
    try:
        raw_headers = sock.recv(23)
        while raw_headers:
            client_id, version, code, size = struct.unpack('!16sBHI', raw_headers)
            headers = RequestHeaders(UUID(bytes=client_id), version, code, size)
            if headers.size > 0:
                content = sock.recv(headers.size)
            else:
                content = b''
            sock.sendall(_get_response(headers, content, db, server_version))
            raw_headers = sock.recv(23)
    except ConnectionError:
        pass
    finally:
        sock.close()
        db.close()
