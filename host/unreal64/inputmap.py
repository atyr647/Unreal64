"""N64 controller state -> host gamepad. The Unreal project sees a normal gamepad; nothing to change."""
from __future__ import annotations

from dataclasses import dataclass

from . import protocol as P


@dataclass
class PadState:
    buttons: dict[str, bool]
    stick_x: float  # -1.0 .. 1.0
    stick_y: float


def decode(payload: bytes) -> PadState:
    raw, sx, sy, _ = P.parse_input(payload)
    # N64 sticks span roughly +/-80 in practice; clamp rather than assume the range.
    norm = lambda v: max(-1.0, min(1.0, v / 80.0))
    return PadState({name: bool(raw >> i & 1) for i, name in enumerate(P.BUTTONS)}, norm(sx), norm(sy))


class Injector:
    """Backend interface. Real backends (Linux uinput, Windows ViGEm virtual Xbox pad) are not written yet."""

    def apply(self, state: PadState) -> None:
        raise NotImplementedError


class LogInjector(Injector):
    def __init__(self):
        self.last = None

    def apply(self, state: PadState) -> None:
        self.last = state
