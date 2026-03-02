import selectors
import socket
from collections.abc import Callable

from db import DB


def _handler_wrapper(request_handler: Callable[[socket.socket, Callable[[], DB], int], None],
                     selector: selectors.BaseSelector,
                     conn: socket.socket, db_factory: Callable[[], DB], server_version: int) -> None:
    try:
        request_handler(conn, db_factory, server_version)
    finally:
        print(f'{conn.getpeername()} - Closing connection')
        selector.unregister(conn)
        conn.close()


# Run the server until a keyboard interrupt.
# New connections are accepted with a selector.
# Reads are also triggered with a selector.
# When receiving a keyboard interrupt, will stop accepting new connections.
def run(ip: str, port: int, server_version: int, db_factory: Callable[[], DB],
        request_handler: Callable[[socket.socket, Callable[[], DB], int], None]) -> None:
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.bind((ip, port))
    sock.listen()
    sock.setblocking(False)
    selector = selectors.DefaultSelector()
    selector.register(sock, selectors.EVENT_READ, {'type': 'accept'})

    print(f'Starting server on {ip}:{port}')
    try:
        while True:
            events = selector.select(timeout=1)
            for key, _ in events:
                if key.data['type'] == 'accept':
                    conn, _ = sock.accept()
                    print(f'{conn.getpeername()} - Accepted connection')
                    conn.setblocking(False)
                    selector.register(conn, selectors.EVENT_READ, {'type': 'handle', 'conn': conn})
                elif key.data['type'] == 'handle':
                    _handler_wrapper(request_handler, selector, key.data['conn'], db_factory, server_version)
                else:
                    print(f'Unknown selector event: {key.data["type"]}')
    except KeyboardInterrupt:
        pass
    finally:
        print('Closing server')
        selector.unregister(sock)
        selector.close()
        if sock:
            sock.close()
