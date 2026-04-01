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

def disassemble(filepath):
    if not os.path.isfile(filepath):
        print(f"Error: File not found: {filepath}")
        return

    with open(filepath, 'rb') as f:
        # 1. Read the 32-bit block count
        count_data = f.read(4)
        if not count_data or len(count_data) < 4:
            print("Error: File is too small to contain block count.")
            return

        block_count = struct.unpack('<I', count_data)[0]
        print(f"Found {block_count} code blocks.")

        # 2. Read the 32-bit offsets
        offsets = []
        for _ in range(block_count):
            offset_data = f.read(4)
            if not offset_data or len(offset_data) < 4:
                print("Error: Reached EOF while reading offsets.")
                return
            offset = struct.unpack('<I', offset_data)[0]
            offsets.append(offset)

        # 3. Iterate over blocks and disassemble
        for i, offset in enumerate(offsets):
            f.seek(offset)

            # Read script name offset (32-bit)
            script_name_offset_data = f.read(4)
            if not script_name_offset_data or len(script_name_offset_data) < 4:
                print(f"Error: Reached EOF while reading script name offset for block {i}.")
                continue
            script_name_offset = struct.unpack('<I', script_name_offset_data)[0]

            # Read 32-bit instruction count
            inst_count_data = f.read(4)
            if not inst_count_data or len(inst_count_data) < 4:
                print(f"Error: Reached EOF while reading instruction count for block {i}.")
                continue
            inst_count = struct.unpack('<I', inst_count_data)[0]

            print(f"\n--- Block {i} (Offset: 0x{offset:X}, Name Offset: 0x{script_name_offset:X}, Instructions: {inst_count}) ---")

            # Read instructions
            for j in range(inst_count):
                inst_data = f.read(4)
                if not inst_data or len(inst_data) < 4:
                    print(f"  Error: Reached EOF while reading instruction {j} of block {i}.")
                    break

                inst = struct.unpack('<I', inst_data)[0]
                opcode = (inst >> 24) & 0xFF
                mnemonic = OPCODES.get(opcode, f"unknown_0x{opcode:02X}")

                print(f"  {j:04d} | 0x{inst:08X} | {mnemonic}")

if __name__ == '__main__':
    filepath = 'dumped_chunks/CODE.bin'
    if len(sys.argv) > 1:
        filepath = sys.argv[1]

    disassemble(filepath)
