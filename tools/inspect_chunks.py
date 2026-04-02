import struct
import sys

def inspect(filepath):
    with open(filepath, 'rb') as f:
        form = f.read(4)
        if form != b'FORM':
            print("Not a FORM file")
            return
        size = struct.unpack('<I', f.read(4))[0]
        print(f"FORM size: {size}")
        
        while True:
            chunk_id = f.read(4)
            if not chunk_id or len(chunk_id) < 4:
                break
            chunk_size = struct.unpack('<I', f.read(4))[0]
            print(f"{chunk_id.decode()} at {hex(f.tell())} size {chunk_size}")
            f.seek(chunk_size, 1)

if __name__ == '__main__':
    inspect(sys.argv[1])
