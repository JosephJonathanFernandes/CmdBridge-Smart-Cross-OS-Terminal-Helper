# Security Policy

## Supported Versions

Currently, the `main` branch is supported with security updates. 

## Reporting a Vulnerability

We take security seriously. If you discover a security vulnerability in this project, please DO NOT report it by creating a public GitHub issue. 

Instead, please send an email to the repository maintainers with a detailed description of the vulnerability. We will attempt to acknowledge your report within 48 hours and work with you to resolve the issue as quickly as possible.

### Safe Input Handling
This repository employs safeguards against command injection and dangerous operations:
- Bound checks are strictly enforced in C.
- Destructive commands (e.g., `rm -rf`, `format`) are intercepted by the `safety.c` module.
- We aim to be fully compliant with GitGuardian scanning standards by not storing any hardcoded secrets. Use `.env` files for secrets instead.
