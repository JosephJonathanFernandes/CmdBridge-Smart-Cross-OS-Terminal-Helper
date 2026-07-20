# CmdBridge

CmdBridge is a smart, cross-platform terminal wrapper that understands plain English. It bridges the gap between natural language and native operating system commands, executing them reliably across Windows, Linux, and macOS.

Instead of googling "how to find large pdf files in powershell", you simply type `find large pdf files`. CmdBridge parses your intent, validates its safety, and executes the correct native OS APIs directly.

```text
Natural Language
        │
        ▼
Quote-aware Parser
        │
        ▼
Intent Engine
        │
        ▼
Validation
        │
        ▼
Safety Engine
        │
        ▼
Mapper
        │
        ▼
Explain Before Execute
        │
        ▼
Native API / Safe Shell
        │
        ▼
Operating System
```

## Project Health

| Metric             | Status                  |
| ------------------ | ----------------------- |
| **Version**        | v0.3.0                  |
| **Commands**       | 10                      |
| **Platforms**      | Windows, Linux, macOS   |
| **Tests**          | 211 Passing             |
| **Coverage**       | 94%                     |
| **Memory Leaks**   | 0                       |
| **CI**             | Passing                 |
| **License**        | MIT                     |

## Why use CmdBridge?

Traditional terminal wrappers translate natural language into blind shell scripts (`bash` or `powershell`), making them dangerously vulnerable to injection attacks and platform inconsistencies. CmdBridge solves this by treating your request as an **Intent** rather than a string of code. 

| Feature                | Status |
| ---------------------- | :----: |
| Quote-aware parser     |    ✅   |
| Dynamic help           |    ✅   |
| Native execution       |    ✅   |
| Explain Before Execute |    ✅   |
| Dry Run                |    ✅   |
| Safety engine          |    ✅   |
| Automated tests        |    ✅   |
| Fuzz testing           |    ✅   |
| CI/CD                  |    ✅   |

## Benchmarks

Benchmarks below were generated using the internal benchmark suite (`cmake --build build --target benchmarks`).

**Machine:** GitHub Actions / Standard Runner (Intel Core i5 Equivalent)  
**Compiler:** GCC / MSVC (Windows 11)

### End-to-End Latency
*(Natural Language → Parser → Mapper → Safety → Decision)*

| Metric   | Time (ms) |
| -------- | --------- |
| Average  | 0.004 ms  |
| Median   | 0.004 ms  |
| P95      | 0.008 ms  |
| Min      | 0.004 ms  |
| Max      | 0.073 ms  |

### Input Length Scaling (Parser)
Demonstrates parser stability under increasing input complexity:

| Length | Time (ms avg) |
| ------ | ------------- |
| 20 chars  | 0.0025 ms |
| 50 chars  | 0.0035 ms |
| 100 chars | 0.0057 ms |
| 500 chars | 0.0192 ms |
| 1000 chars| 0.0331 ms |

### Startup Performance

| Phase | Time (ms) |
| ----- | --------- |
| Load configuration (`commands.conf`) | 0.099 ms |
| Total Startup Time | 0.099 ms |

### Native API Overhead
*(Measuring CmdBridge invocation vs raw OS API call - CreateDirectory)*

| Phase | Time (ms) |
| ----- | --------- |
| OS Raw Call (Average) | 0.353 ms |
| CmdBridge Wrapper (Average) | 0.378 ms |
| **Framework Overhead** | **0.025 ms** |

### Memory

| Metric | Bytes/Count |
| ------ | ----------- |
| Peak Heap | 0 bytes (Stack-based) |
| Memory Leaks | 0 |

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

## Testing

CmdBridge is heavily tested via our automated `CTest` infrastructure on GitHub Actions. It supports full integration sandbox tests, security injection boundaries, fuzz testing, and memory sanitization (ASAN/UBSAN).

To run the tests yourself:
```bash
cd build
ctest --output-on-failure
```

To regenerate benchmarks:
```bash
cmake --build build --target benchmarks
```

## Roadmap

We follow a strict "features over infrastructure" philosophy. Architecture is frozen for `v0.3.x`.

**v0.3.1**
- ZIP support (`zip folder`, `extract zip`)

**v0.3.2**
- Checksums (`sha256`, `md5`, `verify checksum`)

**v0.3.3**
- Git helpers (`git clone`, `git status`)

**v0.4.0**
- Plugin system and Extensibility

## Contributing

We welcome pull requests! If you're adding new commands, simply modify `config/commands.conf` to expose new functionalities. Ensure that all tests pass (`ctest`) before submitting.

## License

MIT License
