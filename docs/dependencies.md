# Dependencies and Tooling

## Core Build Tooling
- CMake 3.16 or newer
- A C++17 compiler such as GCC 11+, Clang 14+, or MSVC 2019+

## GUI
- Qt 6 Widgets

## Testing
- CTest (included with CMake)
- Custom assertion-based unit tests compiled into `chess_tests`

## Recommended Commands
```bash
cmake -S . -B build -DCHESS_ENABLE_GUI=OFF
cmake --build build
ctest --test-dir build --output-on-failure
```
