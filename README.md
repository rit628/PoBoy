# PoBoy

A somewhat accurate Game Boy emulator written in modern C++.

Includes a reusable DMG core written in portable C++ and a basic frontend GUI built with SDL3 (as well as a toy TUI for terminal usage).

## Features
* M-Cycle stepped CPU emulation
* T-Cycle stepped SoC component emulation driven by M-Cycle granular CPU ticks
* Pixel FIFO based PPU emulation
* Fully functional APU emulation with per T-Cycle audio sampling and stereo sound
* Full interrupt, joypad, and timer emulation
* Full DMG bootrom support
* Full support for ROM only cartridges and cartridges with the following bank mapper chips:
    - MBC1 (including battery buffered SRAM)
    - MBC2 (including battery buffered SRAM)
    - MBC3 (including battery buffered SRAM and RTC)
    - MBC5 (including battery buffered SRAM)
* Halt bug emulation
* Generic callback APIs for all major components to connect with an arbitary frontend

## Planned Features
* Additional APIs for PPU VRAM and hardware register access, individual APU channel control, and CPU emulation speed control
* APIs for save state and rewind support
* Serial Emulation
* T-Cycle stepped OAM DMA transfer emulation
* Additional bug emulation (OAM corruption, Wave RAM corruption, etc.)
* Cartridge rumble and gyroscope hardware emulation
* Remaining memory bank controller chip emulation (MBCC6, MBC7, MM01, HuC1, HuC-3)
* CGB emulation
