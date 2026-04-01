import sys
import struct
import os

def validate_win(filepath):
    if not os.path.exists(filepath):
        print(f"File not found: {filepath}")
        return False

    file_size = os.path.getsize(filepath)
    print(f"Validating {filepath} (Size: {file_size} bytes)")

    errors = []

    with open(filepath, 'rb') as f:
        form = f.read(4)
        if form != b'FORM':
            print("Not a FORM file")
            return False

        # Skip FORM size
        f.read(4)

        while True:
            chunk_offset = f.tell()
            chunk_id = f.read(4)

            if not chunk_id or len(chunk_id) < 4:
                break

            try:
                chunk_name = chunk_id.decode('ascii', 'replace')
            except Exception:
                chunk_name = repr(chunk_id)

            chunk_size_data = f.read(4)
            if len(chunk_size_data) < 4:
                break

            chunk_size = struct.unpack('<I', chunk_size_data)[0]
            data_start = f.tell()

            # Chunks we strictly validate
            if chunk_name in ['GEN8', 'STRG', 'TXTR']:
                # Read item count
                count_data = f.read(4)
                if len(count_data) == 4:
                    count = struct.unpack('<I', count_data)[0]
                    # Read offsets
                    for i in range(count):
                        offset_data = f.read(4)
                        if len(offset_data) < 4:
                            errors.append(f"[{chunk_name}] Item {i}: Unexpected end of chunk while reading offset.")
                            break
                        offset = struct.unpack('<I', offset_data)[0]
                        if offset >= file_size:
                            errors.append(f"[{chunk_name}] Item {i}: Invalid offset 0x{offset:08X} (>= file_size {file_size})")
                else:
                    errors.append(f"[{chunk_name}] Could not read item count.")
            elif chunk_name in ['BGND', 'OBJT', 'ROOM', 'AUDO', 'SOND', 'SPRT']:
                # Placeholders for future validation
                pass

            # Advance to the next chunk
            # IFF chunks might be padded to a 4-byte boundary, but typically chunk_size covers the exact data payload.
            # Some parsers align the next chunk to a 4-byte boundary. In standard GM files, chunks are usually tightly packed or already aligned.
            # We'll just seek to the next chunk exactly based on chunk_size.
            next_chunk_pos = data_start + chunk_size
            f.seek(next_chunk_pos)

    if errors:
        print(f"\nValidation failed with {len(errors)} error(s):")
        for error in errors:
            print(f"  - {error}")
        return False
    else:
        print("\nValidation successful! No invalid offsets found.")
        return True

if __name__ == '__main__':
    if len(sys.argv) > 1:
        success = validate_win(sys.argv[1])
        if not success:
            sys.exit(1)
    else:
        print("Usage: validate_win.py <data.win>")
        sys.exit(1)
