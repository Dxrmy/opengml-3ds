def morton(x, y, w):
    return ((y >> 3) * (w >> 3) + (x >> 3)) << 6 | (y & 7) << 3 | (x & 7)

test_coords = [(0,0), (8,0), (0,8), (8,8), (1016, 1016)]
w = 1024

print(f"Morton Offsets for {w}x{w} texture:")
for x, y in test_coords:
    offset = morton(x, y, w)
    print(f"  ({x:4d}, {y:4d}) -> 0x{offset:08X}")
