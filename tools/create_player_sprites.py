from __future__ import annotations

import struct
import zlib
from pathlib import Path


SIZE = 32
TRANSPARENT = (0, 0, 0, 0)
OUTLINE = (7, 11, 24, 255)
SHADOW = (18, 35, 62, 255)
BLUE_DARK = (27, 86, 154, 255)
BLUE = (42, 142, 219, 255)
BLUE_LIGHT = (112, 218, 255, 255)
CANOPY = (167, 244, 255, 255)
CANOPY_DARK = (48, 121, 171, 255)
ENGINE = (252, 183, 64, 255)
ENGINE_CORE = (255, 246, 142, 255)


def blank() -> list[list[tuple[int, int, int, int]]]:
    return [[TRANSPARENT for _ in range(SIZE)] for _ in range(SIZE)]


def set_pixel(pixels, x: int, y: int, color) -> None:
    if 0 <= x < SIZE and 0 <= y < SIZE:
        pixels[y][x] = color


def rect(pixels, x0: int, y0: int, x1: int, y1: int, color) -> None:
    for y in range(y0, y1 + 1):
        for x in range(x0, x1 + 1):
            set_pixel(pixels, x, y, color)


def hline(pixels, y: int, x0: int, x1: int, color) -> None:
    for x in range(x0, x1 + 1):
        set_pixel(pixels, x, y, color)


def draw_center() -> list[list[tuple[int, int, int, int]]]:
    p = blank()

    rows = {
        2: (15, 16),
        3: (14, 17),
        4: (14, 17),
        5: (13, 18),
        6: (13, 18),
        7: (12, 19),
        8: (12, 19),
        9: (11, 20),
        10: (11, 20),
        11: (10, 21),
        12: (10, 21),
        13: (9, 22),
        14: (8, 23),
        15: (7, 24),
        16: (6, 25),
        17: (5, 26),
        18: (4, 27),
        19: (4, 27),
        20: (5, 26),
        21: (7, 24),
        22: (9, 22),
        23: (10, 21),
        24: (11, 20),
        25: (12, 19),
        26: (12, 19),
        27: (13, 18),
    }

    for y, (x0, x1) in rows.items():
        hline(p, y, x0, x1, OUTLINE)
        if x1 - x0 > 2:
            hline(p, y, x0 + 1, x1 - 1, BLUE_DARK if y > 18 else BLUE)

    rect(p, 13, 8, 18, 15, CANOPY_DARK)
    rect(p, 14, 7, 17, 14, CANOPY)
    rect(p, 15, 6, 16, 9, BLUE_LIGHT)

    rect(p, 10, 17, 13, 24, SHADOW)
    rect(p, 18, 17, 21, 24, SHADOW)
    rect(p, 14, 17, 17, 26, BLUE_LIGHT)
    rect(p, 15, 20, 16, 27, CANOPY)

    rect(p, 5, 17, 8, 21, BLUE)
    rect(p, 23, 17, 26, 21, BLUE)
    rect(p, 3, 19, 6, 22, OUTLINE)
    rect(p, 25, 19, 28, 22, OUTLINE)
    rect(p, 6, 18, 9, 20, BLUE_LIGHT)
    rect(p, 22, 18, 25, 20, BLUE_LIGHT)

    rect(p, 11, 27, 13, 29, OUTLINE)
    rect(p, 18, 27, 20, 29, OUTLINE)
    rect(p, 12, 27, 13, 28, ENGINE)
    rect(p, 18, 27, 19, 28, ENGINE)
    set_pixel(p, 13, 29, ENGINE_CORE)
    set_pixel(p, 18, 29, ENGINE_CORE)

    return p


def shifted(source, dx_by_y) -> list[list[tuple[int, int, int, int]]]:
    out = blank()
    for y, row in enumerate(source):
        dx = dx_by_y(y)
        for x, color in enumerate(row):
            if color != TRANSPARENT:
                set_pixel(out, x + dx, y, color)
    return out


def write_png(path: Path, pixels) -> None:
    raw_rows = []
    for row in pixels:
        raw_rows.append(bytes([0]) + b"".join(bytes(pixel) for pixel in row))

    payload = zlib.compress(b"".join(raw_rows), 9)

    def chunk(kind: bytes, data: bytes) -> bytes:
        return (
            struct.pack(">I", len(data))
            + kind
            + data
            + struct.pack(">I", zlib.crc32(kind + data) & 0xFFFFFFFF)
        )

    png = (
        b"\x89PNG\r\n\x1a\n"
        + chunk(b"IHDR", struct.pack(">IIBBBBB", SIZE, SIZE, 8, 6, 0, 0, 0))
        + chunk(b"IDAT", payload)
        + chunk(b"IEND", b"")
    )
    path.write_bytes(png)


def main() -> None:
    target = Path("assets/textures/player")
    target.mkdir(parents=True, exist_ok=True)

    center = draw_center()
    left = shifted(center, lambda y: -1 if y < 17 else (1 if y > 22 else 0))
    right = shifted(center, lambda y: 1 if y < 17 else (-1 if y > 22 else 0))

    write_png(target / "player_ship_center.png", center)
    write_png(target / "player_ship_left.png", left)
    write_png(target / "player_ship_right.png", right)


if __name__ == "__main__":
    main()
