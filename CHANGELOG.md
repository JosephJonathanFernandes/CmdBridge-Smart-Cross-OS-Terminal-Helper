# Changelog

All notable changes to CmdBridge will be documented in this file.

## [v0.3.0] - Initial Release

### Added
- Quote-aware parsing and robust string tokenization.
- Intent engine mapping natural language to distinct actions (`create`, `copy`, `delete`, `search`).
- Cross-platform `EXEC_API` executing filesystem tasks (mkdir, rm, cp, mv) entirely natively in C, bypassing the shell.
- Explain Before Execute mode preventing dangerous commands and surfacing risk profiles safely.
- Dry Run functionality verifying intended actions before execution.
- Dynamic `help` command mapped entirely from a centralized `commands.conf` registry.
- `version` command exposing internal build capabilities, compiler metrics, and supported command statistics.
- Automated benchmark suite compiling explicitly against core source to gather metric measurements for execution latency.

### Changed
- Shifted default platform execution from OS shell proxies to explicit Native C APIs.
- Migrated hardcoded actions to dynamic configuration-driven mapping.

### Security
- Introduced `is_command_safe()` blocking system-destructive parameters like `rm -rf /` or recursive `C:\` deletion.
- Fortified shell execution with strict metacharacter injection blocking (`&`, `|`, `;`, `$`, `` ` ``).
- Integrated AddressSanitizer and UndefinedBehaviorSanitizer directly into the CI pipeline.
- Established 100,000+ iteration Fuzz testing.
