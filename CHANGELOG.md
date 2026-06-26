# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/).

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
