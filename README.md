# CmdBridge

CmdBridge is a smart, cross-platform terminal wrapper that understands plain English. It bridges the gap between natural language and native operating system commands, executing them reliably across Windows, Linux, and macOS.

Instead of googling "how to find large pdf files in powershell", you simply type `find large pdf files`. CmdBridge parses your intent, validates its safety, and executes the correct native OS APIs directly.

## Why use CmdBridge?

Traditional terminal wrappers translate natural language into blind shell scripts (`bash` or `powershell`), making them dangerously vulnerable to injection attacks and platform inconsistencies. CmdBridge solves this by treating your request as an **Intent** rather than a string of code. 

| Feature                | CmdBridge |
| ---------------------- | --------- |
| Cross-platform         | ✅         |
| Native filesystem APIs | ✅         |
| Explain Before Execute | ✅         |
| Dry Run                | ✅         |
| Semantic safety engine | ✅         |
| Automated tests        | ✅         |
| Fuzz tested            | ✅         |
| Open source            | ✅         |

## Performance & Benchmarks

We take speed and reliability seriously. Below are the metrics for **v0.3.0**:

- **Parser average parse time**: 0.38 ms
- **Native execution startup**: 2.1 ms
- **1000 parser fuzz inputs**: 0 crashes
- **Filesystem tests**: 184 passed

## Features

- **Quote-aware Tokenizer**: Handles complex pathing with spaces natively (`copy file "My Resume.pdf" backup`).
- **Explain Before Execute**: Always asks for your explicit permission before mutating your system, explaining precisely what it's about to do.
- **Native OS Execution**: Filesystem operations (`create`, `copy`, `move`, `delete`) bypass the shell entirely, executed natively in C for maximum performance and injection invulnerability.
- **Semantic Safety Engine**: Hard-blocks critical errors like recursive deletion of `C:\` or `/` regardless of how the command was phrased.

## Installation

You can build CmdBridge from source via CMake on Windows, Linux, or macOS.

```bash
git clone https://github.com/JosephJonathanFernandes/CmdBridge-Smart-Cross-OS-Terminal-Helper
cd CmdBridge-Smart-Cross-OS-Terminal-Helper
cmake -B build
cmake --build build
```

## Quick Start

Run the executable to launch the interactive REPL:

```bash
./build/smart_terminal
```

Once inside, use the `help` command to discover capabilities:

```text
> help

CmdBridge v0.3.0

SEARCH
────────────────────────────
find pdf files
search text "TODO"

SYSTEM
────────────────────────────
show running processes
show disk usage

FILES
────────────────────────────
create folder projects
list files
delete folder temp
move file report.pdf docs
copy file report.pdf backup
compress file archive.zip docs

UTILITIES
────────────────────────────
history
explain [command]
help
version
```

## Examples

CmdBridge features an interactive, explicit execution flow.

```text
> create folder test

✓ I understood

Action:
create folders

Source:
test

Platform:
Windows

Method:
Native API (No shell used)

Risk:
SAFE

Will do:
- Create a new empty directory at 'test'.
──────────────────────────────

Proceed? [y/n/d]: y
```

Or you can use Dry Run mode:
```text
Proceed? [y/n/d]: d

--- DRY RUN MODE ---
Would execute:
C API Native Function Call
Nothing executed.
```

## Architecture

CmdBridge employs a strictly layered pipeline:

```text
User
   │
Natural Language
   │
Parser (Extracts Intent & Arguments)
   │
Mapper (Maps intent to OS-specific command/API)
   │
Safety Engine (Validates risk profile)
   │
Explain Before Execute (Human-in-the-loop)
   │
Native API / Safe Execution (Bypasses shell when possible)
   │
Operating System
```

## Supported Platforms

- **Windows 10/11**
- **Ubuntu / Debian Linux**
- **macOS**

## Testing

CmdBridge is heavily tested via our automated `CTest` infrastructure on GitHub Actions. It supports full integration sandbox tests, security injection boundaries, fuzz testing, and memory sanitization (ASAN/UBSAN).

To run the tests yourself:
```bash
cd build
ctest --output-on-failure
```

## Roadmap

**v0.4.0**
- 25+ Supported commands
- Auto-complete

**v0.5.0**
- Fuzzy matching & syntax correction

## Contributing

We welcome pull requests! If you're adding new commands, simply modify `config/commands.conf` to expose new functionalities. Ensure that all tests pass (`ctest`) before submitting.

## License

MIT License
