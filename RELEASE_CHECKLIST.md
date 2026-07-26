# CmdBridge Release Checklist

## 1. Fresh Clone Test
On a clean machine or VM:
```bash
git clone https://github.com/your-username/CmdBridge-Smart-Cross-OS-Terminal-Helper.git
cd CmdBridge-Smart-Cross-OS-Terminal-Helper
cmake -B build
cmake --build build --config Release
ctest --test-dir build -C Release
```
Ensure all tests pass and installation is seamless.

## 2. Build Matrix Verification
Verify build and tests across:
- [ ] Windows (MinGW)
- [ ] Windows (MSVC)
- [ ] Ubuntu (GCC)
- [ ] Ubuntu (Clang)
- [ ] macOS (Apple Clang) (Roadmap)

## 3. Pre-Release Tasks
- [ ] README updated with latest features, commands, and installation instructions.
- [ ] Dictionaries validated (`config/dictionary/*.json`).
- [ ] Version numbers bumped in `CMakeLists.txt` and source files.
- [ ] CI Pipeline is green.

## 4. Release
- [ ] Create and tag release in Git (e.g., `v0.5.0`).
- [ ] Publish binaries/installers for supported platforms.
