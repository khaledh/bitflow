# Bitflow

A hobby 32-bit x86 kernel written in C and assembly.

![Screenshot](screenshot.png)

## Features

- Protected mode with ring 0/3 separation
- Preemptive multitasking with round-robin scheduling
- Keyboard input via PS/2 interrupt handler
- ATA disk driver for loading tasks at runtime
- Simple shell with command execution
- VGA text mode console

## Tutorial

A step-by-step guide to building this kernel from scratch is available in the
[tutorial](tutorial/) folder.

## Building

Requires an i686-elf cross-compiler toolchain (GCC, LD) and NASM.

```
make
```

## Running

```
make run          # curses mode (in terminal)
make run-window   # graphical window
```

## Disk Layout

```
Sector 0     Boot sector
Sector 1     File table (generated at build time)
Sector 2+    Kernel
...          Tasks, fonts
```

The file table maps task names to their disk sectors, so the loader can find
them at runtime without hardcoded offsets.

## Shell Commands

- `help` - list commands
- `about` - version info
- `tasks` - show running tasks
- `task_a`, `task_b` - load sample tasks from disk
- `quit` - shutdown

## License

MIT
