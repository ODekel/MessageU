from dataclasses import dataclass
from uuid import UUID


@dataclass
class RequestHeaders:
    client_id: UUID
    version: int
    code: int
    size: int
