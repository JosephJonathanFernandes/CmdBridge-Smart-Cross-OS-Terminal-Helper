# CmdBridge Testing Infrastructure

This document outlines the automated testing infrastructure for CmdBridge. The testing framework ensures the reliability of the parser, mapper, safety engine, and native OS integrations without modifying the user's local filesystem.

## Running Tests Locally

We use `ctest` driven by `CMake` to execute the full test suite.

### Standard Build & Test
```bash
mkdir build
cd build
cmake ..
cmake --build .
ctest --output-on-failure
```

### Running with Sanitizers (ASAN / UBSAN)
Sanitizers help catch memory leaks and undefined behavior. Supported natively on GCC/Clang (Linux/macOS).
```bash
mkdir build_sanitizers
cd build_sanitizers
cmake .. -DENABLE_SANITIZERS=ON
cmake --build .
ctest --output-on-failure
```

### Generating Coverage Reports
Coverage reports verify that our tests exercise the vast majority of our codebase (Target > 85%).
```bash
mkdir build_coverage
cd build_coverage
cmake .. -DENABLE_COVERAGE=ON
cmake --build .
ctest
lcov --capture --directory . --output-file coverage.info
lcov --remove coverage.info '/usr/*' '*/tests/*' --output-file coverage.info
genhtml coverage.info --output-directory out
```
Then open `out/index.html` in your browser.

## Adding New Tests

1. Create or edit a test file in the `tests/` directory.
2. Include `"test_framework.h"`.
3. Use the macros defined (`ASSERT_TRUE`, `ASSERT_STR_EQ`, `ASSERT_EQ`, etc.).
4. Add your test execution via `RUN_TEST(your_test_function)` inside `run_all_tests()`.
5. If the test requires interacting with the filesystem, invoke `setup_test_sandbox()` and `teardown_test_sandbox()`. This generates a safe `temp_test/` directory.
6. If creating an entirely new test module, register it in `CMakeLists.txt` using the `add_cmdbridge_test` macro.

## Test Types

- **Unit Tests**: Independent validation for parser (`test_parser.c`), mapper (`test_mapper.c`), safety rules (`test_safety.c`), utility functions (`test_utils.c`), and native API execution (`test_native_api.c`).
- **Integration Tests**: End-to-end tests (`integration_tests.c`) ensuring natural language flows flawlessly to a filesystem sandbox.
- **Security Tests**: Automated validations (`security_tests.c`) ensuring shell injection attacks (e.g., `&`, `;`, `$()`) and dangerous intents (`delete /`) are safely rejected without crashing.
- **Fuzz Tests**: A lightweight internal fuzzer (`fuzz_tests.c`) pushing random strings into the parser to ensure no buffer overflows or invalid memory access occur. An LLVM libFuzzer hook is also available.

## Continuous Integration (CI)

Every push and Pull Request automatically triggers our GitHub Actions pipeline (`.github/workflows/ci.yml`). The CI enforces that:
- Builds succeed on Windows and Ubuntu.
- All tests pass (Unit, Integration, Security, Fuzzing).
- Code compiled with memory sanitizers (ASAN/UBSAN) executes cleanly.
- Code coverage is successfully generated.
