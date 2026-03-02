from dataclasses import dataclass
from datetime import datetime
from uuid import UUID


@dataclass
class Client:
    client_id: UUID
    username: bytes
    public_key: bytes
    last_seen: datetime
