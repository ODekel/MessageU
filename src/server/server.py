import selectors
import socket
from collections.abc import Callable
from dataclasses import dataclass


@dataclass
class _EventData:
    handler: Callable[['_EventData'], None]
    selector: selectors.BaseSelector
    sock: socket.socket
    request_handler: Callable[[socket.socket], None]


def _accept(data: _EventData) -> None:
    conn, addr = data.sock.accept()
    print('accepted', conn, 'from', addr)
    conn.setblocking(False)
    data.selector.register(conn, selectors.EVENT_READ, _EventData(_read, data.selector, conn, data.request_handler))


def _read(data: _EventData) -> None:
    data.request_handler(data.sock)
    # TODO: Signal should close connection
    data.selector.unregister(data.sock)
    data.sock.close()


def run(ip: str, port: int, request_handler: Callable[[socket.socket], None]) -> None:
    sel = selectors.DefaultSelector()
    sock = socket.socket()
    sock.bind((ip, port))
    sock.listen()
    sock.setblocking(False)
    sel.register(sock, selectors.EVENT_READ, _EventData(_accept, sel, sock, request_handler))

    while True:
        events = sel.select()
        for key, mask in events:
            callback = key.data.handler
            callback(key.data)
