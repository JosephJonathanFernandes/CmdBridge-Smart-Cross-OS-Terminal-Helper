# CmdBridge Release Checklist

Before tagging a new version, ensure the following steps are fully completed:

- [ ] **README updated**: Ensure all feature flags and supported command counts are accurate.
- [ ] **CHANGELOG updated**: Add the new version, detailing what was Added, Changed, or Fixed.
- [ ] **Tests passing**: Run `ctest` locally and ensure GitHub Actions CI is fully green.
- [ ] **Benchmarks regenerated**: Run `cmake --build build --target benchmarks` and update the README with the latest metrics.
- [ ] **Coverage generated**: Ensure coverage remains high (ideally > 90%).
- [ ] **Version updated**: Update the `version` output string in `src/main.c`.
- [ ] **GitHub Release drafted**: Include binaries (Windows `.exe`, Linux binary) and release notes.
- [ ] **Release tagged**: Tag the commit in git and publish the release.
