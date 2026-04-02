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

def disassemble_v5(filepath):
    with open(filepath, 'rb') as f:
        f.read(4) # FORM
        f.read(4) # size
        
        while True:
            cid = f.read(4)
            if not cid: break
            csize = struct.unpack('<I', f.read(4))[0]
            if cid == b'CODE':
                code_payload_start = f.tell()
                break
            f.seek(csize, 1)
            
        block_count = struct.unpack('<I', f.read(4))[0]
        offsets = struct.unpack('<' + 'I'*block_count, f.read(4 * block_count))
        
        # Block 0
        f.seek(offsets[0])
        name_offset = struct.unpack('<I', f.read(4))[0]
        # Instruction Pointer field is at offsets[0] + 4
        # We read it as a signed int
        rel_ptr = struct.unpack('<i', f.read(4))[0]
        inst_addr = (offsets[0] + 4) + rel_ptr
        
        f.seek(inst_addr)
        print(f"Entrypoint (Block 0) at 0x{inst_addr:X}")
        for j in range(64):
            inst = struct.unpack('<I', f.read(4))[0]
            opcode = (inst >> 24) & 0x7F # v16 uses top 7 bits?
            # Actually let's just print raw hex
            print(f"  {j:04d} | 0x{inst:08X}")

if __name__ == '__main__':
    disassemble_v5("C:\\Users\\kmric\\AppData\\Roaming\\Citra\\sdmc\\3ds\\OpenGML\\data.win")
