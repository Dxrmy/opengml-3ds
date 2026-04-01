import sys
import struct

def inspect_win(filepath):
    with open(filepath, 'rb') as f:
        form = f.read(4)
        if form != b'FORM':
            print("Not a FORM file")
            return
        size = struct.unpack('<I', f.read(4))[0]
        print(f"FORM size: {size}")
        
        while True:
            offset = f.tell()
            chunk_id = f.read(4)
            if not chunk_id or len(chunk_id) < 4:
                break
            chunk_size = struct.unpack('<I', f.read(4))[0]
            print(f"[{offset:08X}] {chunk_id.decode('ascii', 'replace')} : {chunk_size}")
            f.seek(chunk_size, 1)

if __name__ == '__main__':
    if len(sys.argv) > 1:
        inspect_win(sys.argv[1])
    else:
        print("Usage: inspect_win.py <data.win>")