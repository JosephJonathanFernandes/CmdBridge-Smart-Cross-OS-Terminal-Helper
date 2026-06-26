# Smart Terminal Assistant

![CI](https://github.com/JosephJonathanFernandes/CmdBridge-Smart-Cross-OS-Terminal-Helper/actions/workflows/ci.yml/badge.svg)

## The Problem
Navigating the terminal across Windows (PowerShell), macOS, and Linux requires memorizing different command syntaxes for the exact same actions (e.g., `ls -la` vs. `dir`, or `find` vs. `Get-ChildItem`). This causes friction and errors for cross-platform developers.

## The Solution
**Smart Terminal Assistant** is an enterprise-grade CLI tool written in pure C that translates simple natural-language intents into safe, OS-specific terminal commands.

- 🌍 **Cross-Platform**: Dynamically resolves commands using C preprocessor macros (`_WIN32`, `__APPLE__`, `__linux__`).
- 🛡️ **Secure by Default**: Built-in safety module blocks destructive commands (`rm -rf`, `format`, `kill`) before they can harm your system.
- ⚙️ **Configuration-Driven**: Commands are loaded dynamically from `config/commands.conf`, allowing unlimited scalability without touching the core C logic.

## Architecture
See our [Architecture Documentation](docs/ARCHITECTURE.md) for a deep dive into the SOLID design principles used.

## Quick Start

### Prerequisites
- CMake 3.10+
- A C99 compliant compiler (GCC, Clang, MSVC)

### Build & Run
```bash
# Generate build files
cmake -B build

# Compile
cmake --build build

# Run (Windows example)
.\build\smart_terminal.exe
```

## Security
Please review our [SECURITY.md](SECURITY.md) for our vulnerability reporting process and GitGuardian compliance.

## Contributing
We welcome contributions! Read our [CONTRIBUTING.md](CONTRIBUTING.md) to get started.

## License
MIT
