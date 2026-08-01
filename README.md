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
    * MBC1 (including battery buffered SRAM)
    * MBC2 (including battery buffered SRAM)
    * MBC3 (including battery buffered SRAM and RTC)
    * MBC5 (including battery buffered SRAM)
* Halt bug emulation
* APU zombie mode bug emulation
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

# Notes and Usage
At the moment, the frontend is quite primitive as the primary focus of this project was writing a reasonably accurate DMG core. More features will be added once a proper GUI library is added.
* Upon launching the program, a blank window will appear. Click anywhere within the window to open a file selection prompt to select a GB or GBC ROM to run. To change the loaded ROM while a game is running, simply drag and drop your target file into the window and it will be loaded into the emulator.
* Resizing the window will confine the game screen to the maximum integer scale of the GameBoy resolution supported by the current window resolution.
* In supported games, save data is dumped to a file with a matching filename to the ROM with a `.sav` extension whenever the program is closed. If available, save files matching the ROM name within the same directory will be loaded when selected.
* Save data format is a simple dump of the cartridge SRAM, so most emulator save files should be compatible with PoBoy.
* The <kbd>Tab</kbd> key can be held at any time to unlock the emulator's speed for fast forwarding.

## Button Mapping

| Button        | Keyboard                  |
| :------       | :-------                  |
| `DPAD_UP`     | <kbd>&uarr;</kbd>         |
| `DPAD_DOWN`   | <kbd>&darr;</kbd>         |
| `DPAD_LEFT`   | <kbd>&larr;</kbd>         |
| `DPAD_RIGHT`  | <kbd>&rarr;</kbd>         |
| `A`           | <kbd>S</kbd>              |
| `B`           | <kbd>A</kbd>              |
| `START`       | <kbd>Enter</kbd>          |
| `SELECT`      | <kbd>Right Shift</kbd>    |
