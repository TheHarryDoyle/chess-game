# Unit Test Matrix

| Test Case | Area Covered | Expected Outcome |
|---|---|---|
| Initial board setup | Board initialization | 32 pieces in standard opening layout |
| Pawn movement | Core movement rules | One-step and two-step pawn moves succeed from valid states |
| Knight movement | Piece movement | Knights can jump over pieces |
| Blocked sliding piece | Path validation | Bishop and rook cannot move through blockers |
| Wrong-turn rejection | Turn management | Opponent cannot move out of turn |
| Same-color capture rejection | Board integrity | Own pieces cannot be captured |
| Castling | Special move logic | King and rook reposition correctly when legal |
| En passant | Special move logic | Capture only succeeds on the immediate next move |
| Promotion | Special move logic | Pawn auto-promotes to queen on final rank |
| Check detection | Game-state evaluation | Threatened king reported in check |
| Self-check rejection | Full move legality | Moves exposing own king are rejected |
| Checkmate | Terminal-state detection | Forced mate position detected correctly |
| Stalemate | Terminal-state detection | No-legal-move draw detected correctly |
| Engine move generation | Search scaffold | Engine returns a legal move for the side to move |
