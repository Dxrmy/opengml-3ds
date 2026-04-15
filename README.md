<p align="center">
	<a href="#">
		<img src="https://raw.githubusercontent.com/Dxrmy/opengml-3ds/main/etc/logo-alpha.png" alt="OpenGML Icon" width="128">
	</a>
	<h1 align="center">OpenGML (3DS Port)</h1>
	<h5 align="center">Native GML 1.4 Interpreter for the Nintendo 3DS.</h5>
	<p align="center">
		<a href="https://github.com/Dxrmy/opengml-3ds/stargazers">
			<img src="https://img.shields.io/github/stars/Dxrmy/opengml-3ds.svg?style=for-the-badge&color=%237B5BF5" alt="stargazers">
		</a>
		<a href="https://github.com/Dxrmy/opengml-3ds/issues">
			<img src="https://img.shields.io/github/issues/Dxrmy/opengml-3ds?style=for-the-badge&color=%237B5BF5" alt="Issues">
		</a>
		<br>
		<a href="https://en.cppreference.com/w/cpp/17">
			<img src="https://img.shields.io/badge/C%2B%2B-17-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white" alt="C++17">
		</a>
		<img src="https://img.shields.io/badge/Platform-Nintendo%203DS-e60012?style=for-the-badge&logo=nintendo-3ds&logoColor=white" alt="Nintendo 3DS">
		<img src="https://img.shields.io/badge/Status-Active%20Port-success?style=for-the-badge" alt="Active Port">
	</p>
</p>

## 📌 Table of Contents
- [📌 Table of Contents](#-table-of-contents)
- [❓ What is OpenGML 3DS?](#-what-is-opengml-3ds)
- [✨ Features](#-features)
- [🚧 Roadmap & Todo](#-roadmap--todo)
- [🛠 Tech Stack](#-tech-stack)
- [🚀 Getting Started](#-getting-started)
- [⭐ Support the Project](#-support-the-project)
- [📄 License](#-license)

## ❓ What is OpenGML 3DS?

**OpenGML** is a free, open-source, cross-platform interpreter for GameMaker: Studio 1.4. This specific branch is a dedicated native port to the Nintendo 3DS, heavily optimized for the Old 3DS's ARM11 hardware. It allows you to run `.project.gmx` files directly on your handheld.

## ✨ Features  
- **Bytecode Interpreter** – High-performance C++17 bytecode execution tailored for devkitARM.
- **Hardware Optimized** – Utilizes GCC Computed Gotos for maximum ARM11 instruction throughput.
- **Native 3DS Backends** – `citro3d` render loop, `hid` input mapping, and `ndsp` audio integration.
- **Data.win Loader (WIP)** – Direct parsing of compiled GameMaker IFF files for seamless game loading.
- **Developer Suite** – Includes Python-based bytecode disassemblers, IFF validators, and a GML bytecode fuzzer.

## 🚧 Roadmap & Todo
- [x] **Core Interpreter**: C++17 bytecode interpreter ported to `devkitARM`.
- [x] **Display Backend**: `citro2d` sprite and shape rendering functional.
- [x] **Input System**: `hid` keys mapped to GML `vk_` constants.
- [x] **Security Suite**: Fixed buffer overflows and OOB writes in Filesystem core.
- [/] **Data.win Loader**: `FORM`, `GEN8`, `STRG`, `VARI`, and `FUNC` chunk parsing complete.
- [ ] **Audio**: `ndsp` raw PCM buffer implementation.

## 🛠 Tech Stack
- **Languages**: C++17, ARM Assembly, Python (Tooling)
- **Toolchain**: devkitPro / devkitARM
- **Graphics**: citro3d / citro2d
- **Testing**: Catch2

## 🚀 Getting Started

### Prerequisites
- devkitPro with `3ds-dev` and `3ds-sdl2` installed.
- A Nintendo 3DS with Homebrew access or the Citra emulator.

### Building
For detailed build instructions, see [3DS_BUILD.md](3DS_BUILD.md).
```bash
make -f Makefile.3ds
```

## ⭐ Support the Project
Consider giving the project a ⭐ star on GitHub! Your support helps more people discover it and keeps me motivated to improve it.

[![Sponsor Me](https://img.shields.io/badge/Sponsor%20Me-%E2%9D%A4-red?style=for-the-badge)](https://github.com/sponsors/Dxrmy)

---
<p align="center">💻 Ported with ❤️ by <a href="https://github.com/Dxrmy">Dxrmy</a> | 📜 Licensed under MIT</p>
