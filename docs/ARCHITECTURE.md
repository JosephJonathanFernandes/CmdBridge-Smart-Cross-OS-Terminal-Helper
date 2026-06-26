# Architecture Overview

This project is structured using clean code and SOLID principles.

## Module Breakdown

1. **`parser`**: Converts raw user strings into structured `Intent` (Action, Object, Target).
2. **`commands`**: Handles reading the `commands.conf` configuration file to dynamically load supported capabilities.
3. **`os_mapper`**: Evaluates the `Intent` against the loaded templates and dynamically assigns the correct string mapping based on the compile-time target OS.
4. **`safety`**: The gatekeeper. Evaluates output strings against a blacklist of highly destructive operations.
5. **`logger`**: Standardized logging utility for stdout/stderr telemetry.

## Data Flow
`User Input -> parser -> Intent Struct -> os_mapper -> OS Command -> safety -> Output`

## Design Decisions
- **Why C?** C provides extreme performance and portability, ensuring the executable is a tiny dependency-free binary.
- **Why Config-Driven?** Hardcoding templates in C violates the Open/Closed Principle. By shifting command templates to `commands.conf`, users can add new features without recompiling the executable.
