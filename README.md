# OpenGML (3DS Port)

Native GML 1.4 Interpreter for the Nintendo 3DS.

**OpenGML** is a free, open-source, cross-platform interpreter for GameMaker: Studio 1.4. This specific branch is a dedicated native port to the Nintendo 3DS, heavily optimized for the Old 3DS's ARM11 hardware. It allows you to run `.project.gmx` files directly on your handheld.

## The Struggle (Why I built this)

Porting an interpreter to the 3DS was honestly a huge headache, but that's what made it a fun challenge. The Old 3DS's ARM11 processor is brutally slow compared to modern hardware. The hardest part was getting the bytecode dispatch loop fast enough, so I ended up fighting the compiler constantly and had to use GCC Computed Gotos just to squeeze out barely enough instruction throughput. Trying to debug memory alignment issues and OOB writes on this little handheld hardware really tests your patience!

## Features

- **Bytecode Interpreter** - C++17 bytecode execution ported to work with devkitARM.
- **Hardware Optimized** - Uses GCC Computed Gotos to squeeze some extra speed out of the slow ARM11 chip.
- **Native 3DS Backends** - `citro3d` for rendering, `hid` for inputs, and `ndsp` for audio.
- **Data.win Loader (WIP)** - Direct parsing of compiled GameMaker IFF files so games can actually load.
- **Developer Suite** - Python-based disassemblers and fuzzers to help debug the interpreter.

## Roadmap & Todo

- [x] **Core Interpreter**: C++17 bytecode interpreter ported to `devkitARM`.
- [x] **Display Backend**: `citro2d` sprite and shape rendering functional.
- [x] **Input System**: `hid` keys mapped to GML `vk_` constants.
- [x] **Security Suite**: Fixed buffer overflows and OOB writes in Filesystem core.
- [/] **Data.win Loader**: `FORM`, `GEN8`, `STRG`, `VARI`, and `FUNC` chunk parsing complete.
- [ ] **Audio**: `ndsp` raw PCM buffer implementation.

## Tech Stack

- **Languages**: C++17, ARM Assembly, Python (Tooling)
- **Toolchain**: devkitPro / devkitARM
- **Graphics**: citro3d / citro2d
- **Testing**: Catch2

## Getting Started

### Prerequisites

- devkitPro with `3ds-dev` and `3ds-sdl2` installed.
- A Nintendo 3DS with Homebrew access or the Citra emulator.

### Building

For detailed build instructions, see [3DS_BUILD.md](3DS_BUILD.md).
```bash
make -f Makefile.3ds
```
