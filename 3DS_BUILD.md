# Building OpenGML for 3DS

To compile OpenGML for the Nintendo 3DS, you will need the `devkitARM` toolchain installed through `devkitPro`.

## 1. Prerequisites
- [devkitPro](https://devkitpro.org/wiki/Getting_Started) (ensure `3ds-dev`, `3ds-sdl2`, `3ds-sdl2_mixer`, `3ds-sdl2_ttf`, `3ds-citro3d`, and `3ds-citro2d` are installed via `pacman`).
- Ensure the `DEVKITARM` environment variable is set to your `devkitARM` directory.

## 2. Compiling
Run the following command from the `opengml_repo` directory:

```bash
make -f Makefile.3ds
```

This will produce `OpenGML.3dsx` and `OpenGML.elf`.

## 3. Deployment
- **Hardware:** Copy `OpenGML.3dsx` to your SD card (e.g., `/3ds/OpenGML.3dsx`) and launch it from the Homebrew Launcher.
- **Citra:** Launch `OpenGML.elf` or `OpenGML.3dsx` directly in the emulator.
