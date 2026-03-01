import selectors
import socket
from collections.abc import Callable
from threading import Thread

from db import DB


# Run the server until a keyboard interrupt.
# New thread for each connection.
# New connections are accepted with a selector.
# When receiving a keyboard interrupt, will stop accepting new connections,
# but server will only stop once all existing connections have finished.
def run(ip: str, port: int, server_version: int, db_factory: Callable[[], DB],
        request_handler: Callable[[socket.socket, Callable[[], DB], int], None]) -> None:
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.bind((ip, port))
    sock.listen()
    selector = selectors.DefaultSelector()
    selector.register(sock, selectors.EVENT_READ)

    threads = []
    try:
        while True:
            events = selector.select(timeout=1)
            for _, _ in events:
                conn, _ = sock.accept()
                t = Thread(target=request_handler, args=(conn, db_factory, server_version))
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
