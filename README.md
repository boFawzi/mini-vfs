# Mini Virtual File System (mini-vfs)

A console-based virtual file system simulator written in **C (C99)** that runs entirely in user space on Linux. No real disk I/O or kernel modules are involved — all state is held in memory using linked data structures.

Built for **ICS 433 – Operating Systems** at King Fahd University of Petroleum and Minerals.

---

## Features

| Command | Description |
|---|---|
| `mkdir <name>` | Create a new subdirectory |
| `rmdir <name>` | Remove an empty directory |
| `cd <path>` | Navigate (supports `/`, `..`, relative paths) |
| `pwd` | Print current working directory |
| `ls` | List files and subdirectories with sizes |
| `touch <file>` | Create an empty file |
| `write <file> <text>` | Write (overwrite) content to a file |
| `write --append <file> <text>` | Append text to a file |
| `cat <file>` | Display file content |
| `rm <file>` | Delete a file |
| `stat <name>` | Show metadata (type, size, creation time) |
| `help` | List all available commands |
| `exit` | Exit and free all memory |

---

## Requirements

| Requirement | Version |
|---|---|
| OS | Linux (Ubuntu 20.04+ recommended) |
| Compiler | GCC with C99 support (`gcc`) |
| Build tool | `make` |
| C Standard Library | `stdio.h`, `stdlib.h`, `string.h`, `time.h` — no external dependencies |

> No third-party libraries are needed. The project uses only the C standard library.

---

## How to Run

### 1. Clone the repository

```bash
git clone https://github.com/boFawzi/mini-vfs.git
cd mini-vfs
```

### 2. Build the project

```bash
make
```

This compiles all source files under `src/` with `-Wall -Wextra -Wpedantic` and produces the `vfs` executable.

### 3. Run the simulator

```bash
./vfs
```

You will see a shell-style prompt reflecting your current virtual path:

```
/> mkdir home
/> cd home
/home> touch notes.txt
/home> write notes.txt Hello, world!
/home> cat notes.txt
Hello, world!
/home> stat notes.txt
Name:    notes.txt
Type:    file
Size:    13 bytes
Created: Mon May  4 10:23:00 2026
/home> exit
```

### 4. Clean build artifacts

```bash
make clean
```

---

## Project Structure

```
mini-vfs/
├── Makefile
├── include/
│   ├── fs_core.h      # File system types and function declarations
│   ├── parser.h       # Command parser interface
│   └── utils.h        # Path resolution and utilities
└── src/
    ├── main.c         # REPL loop — reads input, dispatches commands
    ├── fs_core.c      # Core FS logic (mkdir, cd, touch, write, …)
    ├── parser.c       # Tokenises raw input into ParsedCmd structs
    └── utils.c        # Path resolution, name validation, prompt generation
```

---

## Architecture Overview

```
User Input (terminal)
        │
        ▼
   main.c  ──►  parser.c/parser.h   →  ParsedCmd
        │
        ├──►  utils.c/utils.h        (path resolve, prompt)
        │
        └──►  fs_core.c/fs_core.h    (all FS operations)
                    │
                    ▼
              FSContext  { root*, cwd* }
              DirEntry   { name, subdirs[], files[], parent* }
              FileEntry  { name, content (heap), size, created }
```

---

## Authors

| ID | Name |
|---|---|
| 202167470 | Abdullah Almajed |
| 202160310 | Abdulaziz Alwarthan |
| 202039560 | Zyad Alyahya |
| 201946290 | Rami Almathani |

---

## Planned Features (Phase 3)

- **Persistence** — save/reload filesystem state to a binary file
- **`cp` / `mv`** — copy and move files between directories
- **`find`** — recursive search by name
- **File size limit** — simulate disk quotas
- **`ls -l`** — detailed listing with timestamps
