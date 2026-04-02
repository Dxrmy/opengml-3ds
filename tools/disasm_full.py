import sys
import struct
import os

OPCODES = {
    0x05: 'push',
    0x06: 'pop',
    0x07: 'call',
    0x08: 'ret',
    0x09: 'jmp',
    0x0A: 'jz',
    0x0B: 'jnz',
}

def disassemble_data_win(filepath):
    if not os.path.isfile(filepath):
        print(f"Error: File not found: {filepath}")
        return

    with open(filepath, 'rb') as f:
        form = f.read(4)
        if form != b'FORM':
            print("Not a FORM file")
            return
        f.read(4) # size
        
        code_offset = None
        while True:
            chunk_id = f.read(4)
            if not chunk_id or len(chunk_id) < 4:
                break
            chunk_size = struct.unpack('<I', f.read(4))[0]
            if chunk_id == b'CODE':
                code_offset = f.tell()
                break
            f.seek(chunk_size, 1)
            
        if code_offset is None:
            print("CODE chunk not found")
            return
            
        # CODE chunk starts with block count
        count_data = f.read(4)
        block_count = struct.unpack('<I', count_data)[0]
        print(f"Found {block_count} code blocks in CODE chunk at 0x{code_offset:X}.")

        offsets = []
        for _ in range(block_count):
            offset = struct.unpack('<I', f.read(4))[0]
            offsets.append(offset)

        # Iterate over first few blocks
        for i in range(min(5, block_count)):
            offset = offsets[i]
            f.seek(offset)

            # Read script name offset (32-bit)
            script_name_offset = struct.unpack('<I', f.read(4))[0]

            # Read 32-bit instruction count
            inst_count = struct.unpack('<I', f.read(4))[0]

            print(f"\n--- Block {i} (Offset: 0x{offset:X}, Name Offset: 0x{script_name_offset:X}, Instructions: {inst_count}) ---")

            # Read instructions
            for j in range(min(64, inst_count)):
                inst = struct.unpack('<I', f.read(4))[0]
                opcode = (inst >> 24) & 0xFF
                mnemonic = OPCODES.get(opcode, f"unknown_0x{opcode:02X}")
                print(f"  {j:04d} | 0x{inst:08X} | {mnemonic}")

if __name__ == '__main__':
    filepath = "C:\\Users\\kmric\\AppData\\Roaming\\Citra\\sdmc\\3ds\\OpenGML\\data.win"
    if len(sys.argv) > 1:
        filepath = sys.argv[1]

    disassemble_data_win(filepath)
