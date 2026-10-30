"""Capture an unmodified game by watching its window/screen with ffmpeg; no engine or project changes."""
from __future__ import annotations

import subprocess
from typing import Iterator

import numpy as np


def ffmpeg_input_args(window_title: str | None = None, display: str = ":0.0") -> list[str]:
    """Input arguments for the current platform. Window capture avoids needing to hide other windows."""
    import platform
    system = platform.system()
    if system == "Windows":
        return ["-f", "gdigrab", "-framerate", "60", "-i", f"title={window_title}" if window_title else "desktop"]
    if system == "Darwin":
        return ["-f", "avfoundation", "-framerate", "60", "-i", "1"]
    return ["-f", "x11grab", "-framerate", "60", "-i", display]


def frames(input_args: list[str], width: int, height: int, fps: int, letterbox: bool = True) -> Iterator[np.ndarray]:
    """Yield (height, width, 3) uint8 RGB frames, scaled to the N64 resolution."""
    if letterbox:
        vf = (f"fps={fps},scale={width}:{height}:force_original_aspect_ratio=decrease:flags=lanczos,"
              f"pad={width}:{height}:(ow-iw)/2:(oh-ih)/2")
    else:
        vf = f"fps={fps},scale={width}:{height}:flags=lanczos"
    cmd = ["ffmpeg", "-loglevel", "error", *input_args, "-vf", vf, "-pix_fmt", "rgb24", "-f", "rawvideo", "-"]
    size = width * height * 3
    with subprocess.Popen(cmd, stdout=subprocess.PIPE) as proc:
        try:
            while True:
                buf = proc.stdout.read(size)
                if len(buf) < size:
                    return
                yield np.frombuffer(buf, dtype=np.uint8).reshape(height, width, 3)
        finally:
            proc.kill()
