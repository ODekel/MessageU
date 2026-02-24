import asyncio
from asyncio import StreamReader, StreamWriter
from collections.abc import Callable


async def _run_server(ip: str, port: int, server_version: int,
                      request_handler: Callable[[StreamReader, StreamWriter, int], None]) -> None:
    server = await asyncio.start_server(lambda r, w: request_handler(r, w, server_version), ip, port)
    async with server:
        await server.serve_forever()


def run(ip: str, port: int, server_version: int,
        request_handler: Callable[[StreamReader, StreamWriter, int], None]) -> None:
    asyncio.run(_run_server(ip, port, server_version, request_handler))
