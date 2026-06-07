# Chess Phase II

## Overview

- C++17 chess core with Qt desktop board prototype
- Supports standard setup, turn control, captures, castling, en passant, auto-queen promotion, check detection, checkmate/stalemate evaluation, and a simple minimax engine scaffold
- Automated tests run through CTest

## Prerequisites

- CMake 3.16+
- C++17 compiler
- Qt 6 Widgets for GUI mode

## Non-GUI Build and Test

```powershell
cmake -S . -B build -DCHESS_ENABLE_GUI=OFF
cmake --build build
ctest --test-dir build --output-on-failure
```

With Visual Studio generators, include the configuration when running tests:

```powershell
ctest --test-dir build -C Debug --output-on-failure
```

## GUI Build Example

```powershell
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_PREFIX_PATH="C:/Qt/6.11.0/mingw_64"
cmake --build build -j 8
.\build\chess_app.exe
```

## Repository Structure

- `include/chess`: public headers
- `src`: chess core implementation
- `src/gui`: Qt window and application entry point
- `tests`: automated rule, state, and engine tests
- `docs`: dependency notes and unit-test matrix
