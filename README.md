# CmdBridge

CmdBridge is a smart, cross-platform terminal engine that bridges the gap between natural language and native operating system commands, executing them reliably across Windows, Linux, and macOS.

Rather than being a simple command wrapper, CmdBridge is a **cross-platform command abstraction engine in C featuring a semantic intermediate representation (Execution IR), capability negotiation, safety validation, adapter-based translation, script migration, structured explainability, regression corpus testing, multi-platform CI, and sanitiser-backed quality assurance.**

## Architecture

At its core, CmdBridge translates intents (not just raw strings) through a strictly layered pipeline. Each stage has a single responsibility and is tested independently.

```text
Input
  │
  ▼
Execution IR (Semantic mapping)
  │
  ▼
Safety (Risk assessment)
  │
  ▼
Capability (Environment negotiation)
  │
  ▼
Adapter (Platform translation)
  │
  ▼
Execution (Native API or Shell)
```

## Features

- **Execution IR**: Maps strings to a semantic target.
- **Layered Translation Pipeline**: Prioritizes Native APIs over OS shells.
- **Explain Before Execute**: Interactive CLI breaks down the exact action, method, and consequences before taking action.
- **Script Migration (`v0.6.0`)**: Transpile legacy Bash scripts to PowerShell (or vice versa) reliably, with structured reporting for unsupported lines.
- **Cross-Platform**: Supports Windows, Linux (POSIX), and macOS.

## Quick Start

You can build CmdBridge from source via CMake.

```bash
git clone https://github.com/JosephJonathanFernandes/CmdBridge-Smart-Cross-OS-Terminal-Helper.git
cd CmdBridge-Smart-Cross-OS-Terminal-Helper
cmake -B build
cmake --build build
```

### 1. Interactive Shell

Run the executable to launch the interactive REPL:

```bash
./build/smart_terminal shell
```

**Example:**
```text
> find large pdf files

✓ I understood

Action:
find files

Target:
large pdf

Platform:
Windows

Method:
System Shell

Risk:
SAFE

Proceed? [y/n/d]: y
```

### 2. Script Migration (`v0.6`)

Need to move a team's build scripts from Linux to Windows?

```bash
cmdbridge migrate build.sh --target-os windows --target-shell powershell --out build.ps1
```

```text
Migration Summary
-----------------
Input lines:   16
Commands:      6
Translated:    6
Approximate:   0
Unsupported:   0
Preserved:     10
```
*(CmdBridge preserves unsupported control-flow blocks and converts commands seamlessly).*

### 3. CLI Tools

```bash
cmdbridge explain "echo hello"
cmdbridge doctor
cmdbridge version
```

## Supported Platforms

| OS | Compilers | CI Status |
| --- | --- | --- |
| **Windows** | MSVC, MinGW | ✅ Active |
| **Linux** | GCC, Clang | ✅ Active |
| **macOS** | Apple Clang | Roadmap |

## Roadmap

The core architecture stabilized in **v0.6.0**. We follow a strict "architecture over features" philosophy.

- **v0.7.0 (Upcoming)**: A stable Plugin API and Plugin Manager. Exposing `CB_Context`, `CB_Intent`, and `CB_Result` handles for extensible development (e.g., Git, Docker, Kubernetes, AI modules).

## Contributing

We welcome pull requests! Ensure that all tests pass (`ctest`) before submitting.
Run tests using:
```bash
cd build
ctest --output-on-failure
```

## License

MIT License
