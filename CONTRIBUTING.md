# Contributing to Smart Terminal Assistant

First off, thank you for considering contributing to the Smart Terminal Assistant! It's people like you that make open-source software such a great community.

## Development Setup
1. Clone the repo and configure CMake:
   ```bash
   cmake -B build
   ```
2. Make your code changes in the `src/` directory. If you are adding a new core capability, update `config/commands.conf`.
3. Add a unit test to `tests/`.

## Quality Standards
We take code quality seriously. Before submitting a PR, ensure:
- Your code is formatted using `clang-format` (we use Google style).
- You haven't hardcoded any configuration or secrets (use `.env` or `.conf` files).
- All tests pass locally (`ctest`).
- Memory is properly managed and checked (run Valgrind if you are on Linux).

## Submitting a PR
- Create a feature branch (`git checkout -b feature/your-feature-name`).
- Provide a clear, descriptive PR title and summary.
- The CI pipeline will automatically lint and test your code. Ensure it passes before requesting a review.
