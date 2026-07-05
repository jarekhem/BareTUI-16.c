# BareTUI-16-C - 8086 Project Context

This file contains the core guidelines and context for the BareTUI-16-C project. It should be used to provide context to the agent in all future conversations.

## Project Overview
- **Goal**: A lightweight Text User Interface (TUI) framework written in C, heavily inspired by classic DOS/AMI BIOS aesthetics.
- **Target Architecture**: 8086 processors (DOS & Bare-metal) and modern PC (Windows/SDL simulator).
- **Toolchain**: Built using CMake. Cross-compiled with `ia16-elf-gcc` for 8086, and standard compilers (MSVC/MinGW) for the Windows Simulator.

## Architecture & Framework Design
- **Object-Oriented C**: Widgets and containers are built using an OOP-like pattern in C. 
  - Inheritance is simulated by placing the base class (e.g., `TUI_Widget base;` or `TUI_Containers_Container base;`) as the **first** member of the struct.
  - Polymorphism is handled via function pointers (`render`, `handle_event`) in the base `TUI_Widget` struct.
- **Event Handling**: The framework uses a **bottom-up (bubbling)** event propagation model. Events are passed to the `focused_child` first. Only if the child does not handle the event (returns `false`), does the parent container process it (e.g., intercepting arrow keys to change tabs).
- **Focus Management**: Containers that hold focusable elements must explicitly set `focusable = true` during their `_init()` so that focus routing logic doesn't drop keyboard events.
- **VGA HAL**: The hardware abstraction layer (`vga_hal.h`) uses far pointers (`__far`) and explicit segment addresses (`0xB800`) on the 8086 to save cycles, while falling back to native pointers on the simulator.

## Coding Standards
- **Naming Conventions**: 
  - Structs and namespaces: `PascalCase` with prefixes indicating hierarchy (e.g., `TUI_Widgets_Checkbox`, `TUI_Containers_TabContainer`).
  - Variables and arguments: `snake_case`.
  - Functions: `Prefix_methodName` (e.g., `TUI_Containers_TabContainer_addTab`).
- **Language**: Standard C. No C++ features. Keep the code compatible with older compilers targeting the 16-bit real-mode architecture.
- **Comments & Documentation**: Always write comments, variables, and function names in English to keep the codebase globally accessible. Keep explanations concise.
- **Performance**: Optimize for low RAM usage and slow CPU cycles. Avoid large runtime allocations where possible; rely on static UI trees built during initialization.

## Build Presets (CMake)
- `windows-native` - Builds the Simulator using a modern compiler.
- `dos-8086` - Cross-compiles for DOS execution using the `ia16-elf-gcc` toolchain.
- `bare-metal-8086` - Cross-compiles for bare-metal execution on 8086 hardware.
