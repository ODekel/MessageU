import asyncio
import selectors
import socket
from asyncio import StreamReader, StreamWriter
from collections.abc import Callable, Awaitable
from threading import Thread


async def _run_server(ip: str, port: int, server_version: int,
                      request_handler: Callable[[StreamReader, StreamWriter, int], Awaitable[None]]) -> None:
    server = await asyncio.start_server(lambda r, w: request_handler(r, w, server_version), ip, port)
    async with server:
        await server.serve_forever()


def run(ip: str, port: int, server_version: int,
        request_handler: Callable[[StreamReader, StreamWriter, int], Awaitable[None]]) -> None:
    asyncio.run(_run_server(ip, port, server_version, request_handler))


def run_with_threads(ip: str, port: int, server_version: int,
                     request_handler: Callable[[socket.socket, int], None]) -> None:
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.bind((ip, port))
    sock.listen()
    sock.setblocking(False)
    selector = selectors.DefaultSelector()
    selector.register(sock, selectors.EVENT_READ)

    threads = []
    try:
        while True:
            events = selector.select(timeout=1)
            for _, _ in events:
                conn, _ = sock.accept()
                t = Thread(target=request_handler, args=(conn, server_version))
                t.start()
                threads.append(t)
    except KeyboardInterrupt:
        pass
    finally:
        if sock:
            sock.close()
        selector.unregister(sock)
        selector.close()
        for t in threads:
            t.join()
