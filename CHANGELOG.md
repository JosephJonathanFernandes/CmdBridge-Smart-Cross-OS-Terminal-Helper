# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/).

## [Unreleased]
### Added
- **Command Explanations**: New `explain` intent provides line-by-line configuration-driven token explanations for CLI beginners.
- **Fuzzy Matching**: Custom non-ML synonym dictionary engine resolves natural conversational commands (e.g. `search my pdfs`) intelligently.
- **Intelligent Defaults**: Parser gracefully assumes target objects based on action intent if omitted.
- **Command History**: All valid inputs are logged securely to `data/history.txt` and easily retrievable using the `history` command.
- **Custom Aliases**: Defined aliases are dynamically loaded, saved to `data/aliases.txt`, and bypass parsing directly to OS execution.
- **Chained Commands**: The central execution engine now supports multi-intent splitting via `&&` separators.
- **Installation Scripts**: Native `build.bat` (Windows) and `Makefile` (Unix) configuration wrappers using CMake install directives.
- **Open Source Polish**: Added `CODE_OF_CONDUCT.md` and MIT `LICENSE`.

## [1.0.0] - 2026-06-26
### Added
- Enterprise folder structure (`src/`, `tests/`, `config/`).
- GitHub Actions CI pipeline.
- Centralized `logger` module.
- Configuration-driven `commands.conf` loading system.
- `SECURITY.md`, `ARCHITECTURE.md`, and `CONTRIBUTING.md`.
- CMake build configuration targeting tests and libraries.

### Changed
- Replaced hardcoded command templates with dynamic loading.
- Replaced `printf` spam with professional leveled logging.
