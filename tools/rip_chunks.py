import sys
import struct
import os

def rip_chunks(filepath):
    out_dir = 'dumped_chunks'
    os.makedirs(out_dir, exist_ok=True)
    with open(filepath, 'rb') as f:
        form = f.read(4)
        if form != b'FORM':
            print("Not a FORM file")
            return
        f.read(4) # size
        
        while True:
            chunk_id = f.read(4)
            if not chunk_id or len(chunk_id) < 4:
                break
            chunk_size = struct.unpack('<I', f.read(4))[0]
            data = f.read(chunk_size)
            name = chunk_id.decode('ascii', 'replace').strip()
            out_path = os.path.join(out_dir, f"{name}.bin")
            with open(out_path, 'wb') as out_f:
                out_f.write(data)
            print(f"Dumped {name}.bin ({chunk_size} bytes)")

if __name__ == '__main__':
    if len(sys.argv) > 1:
        rip_chunks(sys.argv[1])
    else:
        print("Usage: rip_chunks.py <data.win>")