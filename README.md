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

## Profiling and Performance Notes

These measurements were taken locally from a Release build of the non-GUI chess core. Numbers will vary by machine, compiler, build type, and whether the first run is warming up the executable or OS file cache.

| Operation | Local sample size | Iterations per sample | Average | Min | Max | Notes |
| --- | ---: | ---: | ---: | ---: | ---: | --- |
| Board construction/reset | 10 | 100,000 | 0.000030 ms | 0.000028 ms | 0.000034 ms | Creates a board and restores the standard setup |
| Legal pawn move | 10 | 10,000 | 0.000126 ms | 0.000119 ms | 0.000150 ms | Validates and applies `e2` to `e3` from a new board |
| Generate legal moves from start | 10 | 1,000 | 0.012593 ms | 0.012265 ms | 0.013126 ms | Finds legal white moves from the initial position |
| Check detection from start | 10 | 10,000 | 0.000109 ms | 0.000097 ms | 0.000128 ms | Checks whether white is in check from the initial position |
| Engine choose move, depth 1 | 10 | 100 | 0.301029 ms | 0.290723 ms | 0.318817 ms | Searches one ply and evaluates material |
| Engine choose move, depth 2 | 10 | 10 | 6.921550 ms | 6.756220 ms | 7.211120 ms | Searches two plies with alpha-beta pruning |
| Full test executable through CTest | 10 | 1 | 168.398 ms | 133.743 ms | 374.312 ms | Includes test process startup and CTest overhead |

### Compute Cost Model

In this section, "cost" means compute cost: CPU work, memory use, board copies, move generation, and how much work grows as the engine searches deeper positions.

Symbols used below:

- `B` = board squares. For chess, `B = 64`.
- `P` = active pieces on the board. At game start, `P = 32`.
- `M` = legal moves available in the current position. From the starting position, white has about `M = 20`.
- `D` = engine search depth.
- `b` = average branching factor, or average legal moves per position.
- `N` = number of positions visited by the engine search.

Request path for a normal GUI move:

```text
Player click
  -> ChessWindow::handleCellClick(...)
  -> Board::movePiece(move)
      -> validate piece movement
      -> validate path for sliding pieces
      -> reject same-color captures
      -> simulate the move on a copied board
      -> reject moves that leave own king in check
      -> apply special move handling when needed
  -> ChessWindow::refreshBoard()
      -> redraw 64 board cells
  -> ChessWindow::updateStatusPanel(...)
      -> check game state
      -> show turn, check, checkmate, or stalemate
  -> ChessEngine::chooseMove(...) for black
      -> generate legal moves
      -> minimax search with alpha-beta pruning
      -> apply selected engine move
```

Approximate compute cost for a player move:

| Stage | Work Per Move | Cost Shape |
| --- | --- | --- |
| Board lookup | Reads one source square and one target square | `O(1)` |
| Piece movement validation | Checks the selected piece's movement rule | Usually `O(1)` |
| Sliding path validation | Rook, bishop, and queen scan along a line | Worst case `O(8)`, effectively constant on an 8x8 board |
| Self-check prevention | Copies the board and checks king safety | About `O(B + attack checks)` |
| Special move handling | Castling, en passant, and promotion checks | `O(1)` |
| GUI board refresh | Rewrites all board cells | `O(B)`, with `B = 64` |
| Game-state update | Checks check, checkmate, or stalemate | Usually dominated by legal move generation |

Approximate compute cost for engine search:

| Stage | Work Per Engine Move | Cost Shape |
| --- | --- | --- |
| Root legal move generation | Finds legal moves for the side to move | `O(P * candidate moves * validation cost)` |
| Board copy per candidate | Copies board state before trying a move | `O(B)` per candidate |
| Minimax expansion | Searches future positions | Roughly `O(b^D)` before pruning |
| Alpha-beta pruning | Skips branches once bounds prove they cannot improve the result | Best case much less than `O(b^D)`, worst case still exponential |
| Material evaluation | Scans the board and sums piece values | `O(B)` per leaf position |

With the current default engine depth of `D = 2`, the measured local cost was about `6.9 ms` per engine move from the starting position. That is fast enough for the current desktop prototype. Increasing depth is the main way to make the app feel slower, because each extra ply multiplies the number of board positions the engine may need to inspect.

A rough search-size estimate is:

```text
positions_to_evaluate ~= b^D
```

Using the opening branching factor of about `b = 20`:

```text
depth 1 ~= 20 positions
depth 2 ~= 400 positions
depth 3 ~= 8,000 positions
depth 4 ~= 160,000 positions
```

Alpha-beta pruning can reduce that, but the shape is still the same: engine depth is the expensive setting.

Memory cost:

- The board is small: 64 squares plus turn, castling, and en passant state.
- Normal move validation uses temporary board copies to test whether a move leaves the king in check.
- Engine search creates many temporary board copies while exploring possible moves.
- GUI refresh cost is stable because the interface always redraws the same 64 squares.
- There is no database or file-backed game history in the current version, so memory does not grow over time during normal play.

Practical optimization ideas:

- Keep the default engine depth low for responsive GUI play.
- Avoid unnecessary full-board copies in hot paths.
- Cache generated legal moves only when board state has not changed.
- Add move ordering so alpha-beta pruning cuts off more branches earlier.
- Add a transposition table if deeper engine search is added later.
- Consider piece-square tables before raising depth, because evaluation quality can improve without multiplying search cost as much.

### Performance Bottlenecks to Watch

The current chess core is small and predictable. Performance will mostly depend on:

- Engine depth.
- Number of legal moves in the current position.
- How often legal moves are regenerated for checkmate and stalemate checks.
- Board copies made during validation and minimax.
- Whether the app is built in Debug or Release mode.

For a class project and desktop prototype, this design is reasonable: normal move validation is very fast, the board is tiny, and the simple engine gives automated play without requiring a heavyweight chess engine dependency.

### Future Profiling Improvements

Good next steps from a performance engineering standpoint:

- Add a small benchmark target that can be run intentionally from CMake.
- Track positions visited by the minimax engine.
- Log engine decision time per move in the status panel or debug output.
- Compare Debug and Release timings.
- Benchmark depth 1, 2, 3, and 4 before raising the default depth.
- Add tests around engine move timing if responsiveness becomes a requirement.

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
