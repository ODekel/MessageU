import struct
from asyncio import StreamReader, StreamWriter
from collections.abc import Callable
from uuid import UUID

from models import RequestHeaders, ServerException
from request_handlers import register_client, get_client_list, get_client_public_key, send_message, get_messages


_CODE_TO_HANDLER_MAPPING: dict[int, Callable[[RequestHeaders, bytes], bytes]] = {
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


def _get_response(headers: RequestHeaders, content: bytes, server_version: int) -> bytes:
    try:
        res_content = _CODE_TO_HANDLER_MAPPING.get(headers.code, _unknown_code)(headers, content)
        res_code = _CODE_TO_RES_CODE_MAPPING[headers.code]
    except ServerException:
        res_content = b''
        res_code = _ERROR_RES_CODE
    return struct.pack('!BHI', server_version, res_code, len(res_content)) + res_content


async def handler(reader: StreamReader, writer: StreamWriter, server_version: int) -> None:
    raw_headers = await reader.readexactly(23)
    client_id, version, code, size = struct.unpack('!16sBHI', raw_headers)
    headers = RequestHeaders(UUID(bytes=client_id), version, code, size)
    if headers.size > 0:
        content = reader.readexactly(headers.size)
    else:
        content = b''
    writer.write(_get_response(headers, content, server_version))
    await writer.drain()
