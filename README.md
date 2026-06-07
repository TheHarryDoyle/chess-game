# Chess Game

A C++17 chess game with a Qt desktop interface, full board-state validation, special chess moves, check/checkmate detection, automated tests, and a simple minimax-based computer move system.

## Features

- Standard chess starting position
- Legal move validation for all pieces
- Turn tracking for white and black
- Captures and same-color capture prevention
- Castling
- En passant
- Automatic pawn promotion to queen
- Check, checkmate, and stalemate detection
- Basic minimax engine for computer responses
- Qt desktop board interface
- CTest-based unit tests

## How to Use

The GUI opens an interactive chess board. White moves first. Click one of your pieces, then click a destination square. If the move is legal, the board updates and the game state is recalculated.

After the player moves, the engine can choose a response for black. The status panel reports whose turn it is, whether the game is active, in check, checkmate, or stalemate, and whether a selected move was accepted or rejected.

Toolbar buttons:

- `New Game`: starts a fresh game
- `Reset Board`: resets the board to the starting position
- `Run Tests`: runs the compiled `chess_tests` executable from the build output
- `Exit`: closes the application

## How It Looks

The desktop app uses a clean Qt window with:

- An 8x8 chess board labeled with files `a-h` and ranks `1-8`
- Alternating light and dark tan squares
- Unicode chess pieces centered inside each square
- A green highlight on the currently selected square
- A top toolbar for game actions
- A right-side status panel showing turn, engine state, game state, and move messages

The main window title is `Chess Application`, and the default layout is sized for a large board with a compact status area beside it.

## Build Requirements

- CMake 3.16 or newer
- A C++17 compiler
- Qt 6 Widgets if building the GUI

The core chess logic and tests can be built without Qt by disabling the GUI.

## Build and Run Tests Without GUI

```powershell
cmake -S . -B build -DCHESS_ENABLE_GUI=OFF
cmake --build build
ctest --test-dir build --output-on-failure
```

If you are using a Visual Studio generator, include the build configuration when running tests:

```powershell
ctest --test-dir build -C Debug --output-on-failure
```

## Build and Run the GUI

Example for Windows with MinGW and Qt 6:

```powershell
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_PREFIX_PATH="C:/Qt/6.11.0/mingw_64"
cmake --build build -j 8
.\build\chess_app.exe
```

If Qt is installed somewhere else, update `CMAKE_PREFIX_PATH` to match your Qt installation.

## Performance Cost

The main performance cost comes from move generation and the minimax engine.

- Board updates are small because the board is only 8x8.
- Rule validation is lightweight for normal player moves.
- The engine copies board states while searching possible moves.
- Search cost grows quickly as depth increases because each legal move creates more possible future positions.
- The default engine depth is low, so responses stay practical for a class project and desktop prototype.

In short, the GUI and normal move validation are inexpensive. The engine search is the part that becomes costly if the depth is increased.

## Project Structure

```text
include/chess/   Public headers
src/             Chess core implementation
src/gui/         Qt window and GUI entry point
tests/           Unit tests
docs/            Dependency notes and unit test matrix
```

## Tests

The test suite covers:

- Initial board setup
- Pawn, knight, rook, bishop, queen, and king movement behavior
- Blocked paths
- Wrong-turn rejection
- Same-color capture rejection
- Castling
- En passant
- Promotion
- Check detection
- Self-check prevention
- Checkmate
- Stalemate
- Engine legal move generation

More detail is available in `docs/unit_test_matrix.md`.
