import sys
import struct
import os

def parse_chunks(f):
    chunks = {}
    f.seek(0)
    form = f.read(4)
    if form != b'FORM':
        return None

    f.read(4) # Total size

    while True:
        chunk_id = f.read(4)
        if not chunk_id or len(chunk_id) < 4:
            break
        chunk_size = struct.unpack('<I', f.read(4))[0]
        chunks[chunk_id.decode('ascii', 'replace').strip()] = {
            'offset': f.tell(),
            'size': chunk_size
        }
        # Skip to the next chunk
        f.seek(chunk_size, os.SEEK_CUR)

    return chunks

def extract_textures(filepath):
    with open(filepath, 'rb') as f:
        chunks = parse_chunks(f)
        if not chunks:
            print(f"Error: {filepath} is not a valid FORM file.")
            return

        if 'TXTR' not in chunks:
            print("Error: No TXTR chunk found in file.")
            return

        txtr = chunks['TXTR']
        txtr_start = txtr['offset']
        txtr_end = txtr_start + txtr['size']

        f.seek(txtr_start)
        count = struct.unpack('<I', f.read(4))[0]

        # Read the texture item offsets
        item_offsets = []
        for _ in range(count):
            offset = struct.unpack('<I', f.read(4))[0]
            item_offsets.append(offset)

        png_data = []
        for i, offset in enumerate(item_offsets):
            f.seek(offset)
            _flag = struct.unpack('<I', f.read(4))[0]
            png_offset = struct.unpack('<I', f.read(4))[0]
            png_data.append((i, png_offset))

        # Sort by png_offset so we know where each one ends
        png_data_sorted = sorted(png_data, key=lambda x: x[1])

        out_dir = 'extracted_textures'
        os.makedirs(out_dir, exist_ok=True)

        for j, (original_index, png_offset) in enumerate(png_data_sorted):
            f.seek(png_offset)
            magic = f.read(4)
            if magic != b'\x89PNG':
                print(f"Warning: Missing PNG magic number at offset {png_offset} for texture {original_index}. Skipping.")
                continue

            # Determine end boundary
            if j < len(png_data_sorted) - 1:
                next_png_offset = png_data_sorted[j + 1][1]
                size = next_png_offset - png_offset
            else:
                size = txtr_end - png_offset

            f.seek(png_offset)
            data = f.read(size)

            out_path = os.path.join(out_dir, f"tex_{original_index}.png")
            with open(out_path, 'wb') as out_f:
                out_f.write(data)

            print(f"Extracted tex_{original_index}.png ({size} bytes)")

if __name__ == '__main__':
    if len(sys.argv) > 1:
        extract_textures(sys.argv[1])
    else:
        print("Usage: python extract_textures.py <data.win>")
