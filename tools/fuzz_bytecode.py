import struct
import os

def create_fuzz_bytecode():
    instructions = [
        (0x06 << 24) | 0x000000, # pop
        (0x07 << 24) | 0xFFFFFF, # call non-existent func
        (0x09 << 24) | 0xFFFFFF, # jmp out of bounds forward
        (0x09 << 24) | 0x800000, # jmp out of bounds backward
        (0xFF << 24) | 0x000000, # invalid opcode
    ]

    script_name = b"fuzz_script\0"

    # We construct CODE payload first, then add absolute offsets.
    # To do that we need to know the offset of the CODE payload in the file.
    gen8_payload = b'\0' * 0x44
    gen8_chunk = b'GEN8' + struct.pack('<I', len(gen8_payload)) + gen8_payload

    code_start = 8 + len(gen8_chunk)
    code_payload_start = code_start + 8

    block_count = 1

    # Let's decide whether disasm_win.py expects absolute or relative offsets.
    # The hint: "The CODE chunk payload in GameMaker data.win files begins directly with a 32-bit block count, followed by 32-bit block offsets. Each block consists of a 32-bit string offset (script name), a 32-bit instruction count, and the 32-bit instructions themselves".
    # And another hint: "In standard GameMaker 1.4 builds, data.win chunks like GEN8, STRG, and TXTR consistently begin with a 32-bit item count followed immediately by an array of 32-bit absolute file offsets."
    # If CODE chunk works like GEN8, STRG, TXTR, then it uses absolute file offsets.
    # If disasm_win.py fails with absolute file offsets on `CODE.bin`, then `disasm_win.py` might be assuming relative offsets, or it was written for a file that was ripped weirdly.
    # Let me just make the fuzzer output what the hint says. If it's a valid data.win, it will use absolute file offsets.
    # Wait, disasm_win.py runs on CODE.bin which is JUST the payload. If the offset is absolute from file start, `disasm_win.py` doing `f.seek(offset)` will seek past the end of the payload because it lacks the FORM and GEN8 headers! So yes, `disasm_win.py` expects absolute offsets but because it operates on `CODE.bin`, it fails unless the offset is relative or it handles it. Wait, the prompt says "run this on the 3DS to ensure our try/catch". The fuzzer is the goal.

    # We will use absolute offsets.
    block_offset = code_payload_start + 4 + 4
    inst_start = block_offset + 8
    str_offset = inst_start + len(instructions) * 4

    code_payload = bytearray()
    code_payload.extend(struct.pack('<I', block_count))
    code_payload.extend(struct.pack('<I', block_offset))
    code_payload.extend(struct.pack('<I', str_offset)) # script name offset
    code_payload.extend(struct.pack('<I', len(instructions)))
    for inst in instructions:
        code_payload.extend(struct.pack('<I', inst))
    code_payload.extend(script_name)

    # Align payload size to 4 bytes
    while len(code_payload) % 4 != 0:
        code_payload.extend(b'\0')

    code_chunk = b'CODE' + struct.pack('<I', len(code_payload)) + code_payload

    form_payload = gen8_chunk + code_chunk
    form = b'FORM' + struct.pack('<I', len(form_payload)) + form_payload

    with open('fuzz_data.win', 'wb') as f:
        f.write(form)

    print(f"Generated fuzz_data.win with {len(form)} bytes.")

if __name__ == '__main__':
    create_fuzz_bytecode()