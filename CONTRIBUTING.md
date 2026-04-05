# Contributing

Thanks for your interest in improving TerminalChess-AI.

## Development Setup
1. Install a C++17 toolchain and CMake 3.20+.
2. Configure and build:
   - `cmake -S . -B build -DCMAKE_BUILD_TYPE=Release`
   - `cmake --build build --config Release`

## Validation
Run these checks before opening a PR:
- `./build/terminalchess_ai --help`
- `./build/terminalchess_ai --version`

On Windows:
- `.\\build\\Release\\terminalchess_ai.exe --help`
- `.\\build\\Release\\terminalchess_ai.exe --version`

## Pull Request Guidelines
- Keep changes focused and atomic.
- Update `README.md` and `docs/ARCHITECTURE.md` when behavior changes.
- Preserve compatibility of existing CLI options unless intentionally versioned.
