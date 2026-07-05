# BareTUI-16-C

BareTUI-16-C is an object-oriented Text User Interface (TUI) framework written in pure C. It is specifically designed for 16-bit 8086 architectures (running as DOS executables or Bare-metal applications), while also featuring a native Windows Simulator using SDL2 for easy development.

Heavily inspired by classic DOS and AMI BIOS aesthetics, the engine brings an elegant and highly optimized UI framework to retro hardware.

## Features
- **Object-Oriented C:** Widgets and containers use a virtual table-like structure for `render` and `handle_event` methods.
- **Event Bubbling:** Modern bottom-up event propagation to efficiently handle keyboard focus and container actions.
- **Hardware Abstraction Layer (HAL):** Includes a VGA text-mode implementation (using `__far` pointers) optimized for real-mode 8086, as well as a fallback SDL2 renderer for modern PCs.
- **Classic UI Elements:** Tabs, Checkboxes, Buttons, Radio buttons, Panels, and classic VGA text-mode coloring.

## Build Requirements
To compile and test this project across all target platforms, you will need the following tools. **Ensure they are added to your system's `PATH` variable:**

* **CMake** (v3.15 or newer)
* **ia16-elf-gcc**: GCC toolchain specifically built for 16-bit 8086 architectures.
* **Bochs**: Required to test and emulate the bare-metal builds.
* **DOSBox-X**: Required to run and test the compiled MS-DOS executables.
* **MinGW-w64 / GCC**: Required to compile the Windows Simulator.

*Note: The Windows simulator uses `FetchContent` to download precompiled SDL2 MinGW binaries automatically during configuration.*

## Build Instructions (CMake Presets)

The project leverages `CMakePresets.json` to handle multiple target environments:

### 1. Windows Simulator (Native)
Builds the project as a standard Windows executable using SDL2 for rapid UI prototyping.
```bash
cmake --preset windows-native
cmake --build --preset windows-native
```

### 2. DOS 8086
Cross-compiles the framework into a real-mode MS-DOS executable.
```bash
cmake --preset dos-8086
cmake --build --preset dos-8086
```

### 3. Bare-Metal 8086
Cross-compiles the framework to be booted directly from a ROM/floppy image without any underlying operating system.
```bash
cmake --preset bare-metal-8086
cmake --build --preset bare-metal-8086
```

## Running the Emulators

The CMake configuration includes custom targets to automatically launch your compiled binaries using the emulators. For example, to run the Bare-metal build directly in Bochs:

```bash
cmake --build --preset bare-metal-8086 --target run_bare-metal
```

## Acknowledgments & Licensing

- The core **BareTUI-16-C framework** (all `.c` and `.h` files) is released under the **MIT License**.
- The **bare-metal emulator environment** (`Simulator/platform/bare-metal/*.asm`) incorporates code adopted from the open-source XT-clone BIOS (notably maintained by Sergey Kiselev and Anonymous). These specific assembly files are licensed under the **GNU General Public License v3.0 (GPLv3)**. Consequently, any compiled bare-metal binary that links against these BIOS files is subject to the terms of the GPLv3.
