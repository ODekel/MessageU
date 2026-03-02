from dataclasses import dataclass
from uuid import UUID


@dataclass
class Message:
    message_id: int
    to_client: UUID
    from_client: UUID
    type: int
    content: bytes
