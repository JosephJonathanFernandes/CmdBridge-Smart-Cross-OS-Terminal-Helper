# CmdBridge Architecture

CmdBridge is designed as a **cross-platform command compatibility framework**. It allows users to write commands in the vernacular they know best (e.g., Linux `ls`, Windows `dir`, or natural language), and the tool safely translates and executes them on any underlying system.

The core design philosophy avoids direct 1-to-1 string substitution. Instead, it relies on an **Intent Engine** that parses input into an intermediate representation (IR).

## Data Flow Pipeline

The execution of a command follows a strict pipeline:

```text
User Input
    ↓
[ Translator ]
    ↓
Execution IR (Intermediate Representation)
    ↓
[ Safety Analyzer ]
    ↓
[ Capability Resolver ]
    ↓
[ Adapter ]
    ↓
Native Command
    ↓
[ Execution ]
```

### 1. Translator
Converts raw user input into an `ExecutionIR`. It uses dictionaries to match known verbs and flags to semantic operations (e.g., `INTENT_LIST_DIRECTORY`, `INTENT_DELETE_FILE`) and flags (e.g., `recursive`, `force`).

### 2. Safety Analyzer
Takes the `ExecutionIR` and assigns a risk level (e.g., `SAFE`, `WARNING`, `DANGEROUS`). This ensures that destructive operations like `rm -rf /` are caught semantically, regardless of whether the user typed `rm` or `del`.

### 3. Capability Resolver
Checks if the current operating system and shell can fulfill the requested `ExecutionIR`. It handles feature negotiation (e.g., if a shell does not support showing hidden files, this stage detects it).

### 4. Adapter
Converts the `ExecutionIR` into a native string suitable for the target OS and Shell. It generates an `AdapterScore` indicating confidence and native compatibility.

## Key Abstractions

- `ExecutionIR`: The core data structure representing *what* the user wants to do, completely divorced from *how* the operating system will do it.
- `EnvironmentInfo`: Encapsulates detection of the host OS and Shell, using a layered fallback mechanism (CLI args -> Parent Process ID -> Environment Variables -> Default).

## Extending CmdBridge

Because of the IR architecture, adding support for a new shell (e.g., Fish, Nushell) or OS simply requires adding a new dictionary JSON file in `config/dictionary/`. The core C code does not need to be recompiled.
