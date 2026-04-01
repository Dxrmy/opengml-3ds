import sys
import struct
import os

def validate_win(filepath):
    if not os.path.isfile(filepath):
        print(f"Error: File '{filepath}' not found.")
        sys.exit(1)

    physical_file_size = os.path.getsize(filepath)
    corruptions = []

    with open(filepath, 'rb') as f:
        form = f.read(4)
        if form != b'FORM':
            print(f"Error: Not a FORM file (found {form}).")
            sys.exit(1)

        f.read(4) # size

        while True:
            chunk_start_pos = f.tell()
            chunk_id = f.read(4)
            if not chunk_id or len(chunk_id) < 4:
                break

            chunk_size_bytes = f.read(4)
            if len(chunk_size_bytes) < 4:
                break

            chunk_size = struct.unpack('<I', chunk_size_bytes)[0]
            chunk_name = chunk_id.decode('ascii', 'replace').strip()

            # Phase 1: Chunk Bounds Validation
            if chunk_start_pos + 8 + chunk_size > physical_file_size:
                corruptions.append(f"Chunk {chunk_name} at offset {chunk_start_pos} with size {chunk_size} extends past file end.")
                # We can choose to break or continue; seeking might fail, but let's just seek to what we can or break.
                # If the chunk is out of bounds, skipping it via seek will probably fail, so let's just break the loop here.
                break

            payload_start_pos = f.tell()

            # Phase 2: Deep Pointer Validation
            try:
                if chunk_name == 'GEN8':
                    # Seek to offset 0x28 relative to the chunk payload start? Or chunk start?
                    # "Seek to offset 0x28 relative to the chunk start"
                    f.seek(chunk_start_pos + 0x28)
                    offset_bytes = f.read(4)
                    if len(offset_bytes) == 4:
                        name_offset = struct.unpack('<I', offset_bytes)[0]
                        if name_offset >= physical_file_size:
                            corruptions.append(f"GEN8 Game Name offset out of bounds: points to {hex(name_offset)}")

                elif chunk_name == 'STRG':
                    f.seek(payload_start_pos)
                    count_bytes = f.read(4)
                    if len(count_bytes) == 4:
                        count = struct.unpack('<I', count_bytes)[0]
                        for i in range(count):
                            offset_bytes = f.read(4)
                            if len(offset_bytes) < 4:
                                break
                            offset = struct.unpack('<I', offset_bytes)[0]
                            if offset >= physical_file_size:
                                corruptions.append(f"STRG offset {i} out of bounds: points to {hex(offset)}")

                elif chunk_name == 'TXTR':
                    f.seek(payload_start_pos)
                    count_bytes = f.read(4)
                    if len(count_bytes) == 4:
                        count = struct.unpack('<I', count_bytes)[0]
                        for i in range(count):
                            offset_bytes = f.read(4)
                            if len(offset_bytes) < 4:
                                break
                            txtr_item_offset = struct.unpack('<I', offset_bytes)[0]

                            # Save current position in offset array
                            saved_pos = f.tell()

                            if txtr_item_offset + 8 > physical_file_size:
                                corruptions.append(f"TXTR item {i} offset out of bounds: points to {hex(txtr_item_offset)}")
                            else:
                                f.seek(txtr_item_offset)
                                f.read(4) # Skip dummy flag
                                png_offset_bytes = f.read(4)
                                if len(png_offset_bytes) == 4:
                                    png_offset = struct.unpack('<I', png_offset_bytes)[0]
                                    if png_offset >= physical_file_size:
                                        corruptions.append(f"TXTR item {i} PNG offset out of bounds: points to {hex(png_offset)}")

                            # Restore position
                            f.seek(saved_pos)
            except Exception as e:
                corruptions.append(f"Error parsing chunk {chunk_name}: {e}")

            # Advance to next chunk
            f.seek(payload_start_pos + chunk_size)

    if corruptions:
        for c in corruptions:
            print(c)

if __name__ == '__main__':
    if len(sys.argv) > 1:
        validate_win(sys.argv[1])
    else:
        print("Usage: validate_win.py <data.win>")
