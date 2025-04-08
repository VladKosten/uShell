# uShell

## About uShell

uShell is a lightweight and efficient command-line interface (CLI) tailored for embedded systems such as microcontrollers. It provides a simple yet powerful way to interact with and control various system features.

## Project Structure

- **main/**
  Contains the core implementation of uShell, including the main loop and initialization routines. See [ushell.c](main/ushell.c) and [ushell.h](main/ushell.h). [ushell_vcp.c](main/ushell_vcp.c) is a virtual COM port implementation for serial communication.

- **cmd/**
  Houses the command implementations.
  - **fs_littlefs/**
    Implements file system commands using the LittleFS file system (e.g., [ushell_cmd_fs.c](cmd/fs_littlefs/ushell_cmd_fs.c)).
  - **whoami/**, **help/**, etc.
    Provide additional CLI commands.

- **port/**
  Contains hardware abstraction layers:
  - **hal/**: Hardware-specific implementations.
  - **osal/**: Operating system abstraction.

- **startup/**
  Contains startup routines and system initialization. See [ushell_startup.c](startup/ushell_startup.c).

- **test/**
  IN PROGRESS: This directory is intended for unit tests and test frameworks.

- **utils/**
  XModem implementation for file transfer. See [ushell_xmodem.c](utils/ushell_xmodem.c).

## Coding Style

The project adheres to the coding style defined in the [.clang-format](.clang-format) file. This ensures consistency in spacing, pointer alignment, and overall formatting across the codebase.

## Key Features

- **Intuitive CLI:**
  A user-friendly command-line interface for interacting with the system.
- **Filesystem Operations:**
  Supports file read/write operations, including file transfers using XModem.
- **Modular Architecture:**
  Separates command definitions, hardware abstraction, and utilities for ease of maintenance.
- **Thread-Safe:**
  Designed to be thread-safe, allowing for concurrent operations in multi-threaded environments.
- **Abstracted layers:**
  Abstracts hardware and operating system layers, making it portable across different platforms.
- **Customizable:**
    Easily extendable with new commands and features.

## Licence

This project is licensed under the terms specified in [LICENCE.md](LICENCE.md).

## Authors

- **Vladislav Kosten**
  Email: vladkosten@gmail.com