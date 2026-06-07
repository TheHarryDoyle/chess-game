#include "chess/MoveValidator.h"

#include <cmath>

namespace chess {

namespace {
bool isOpponent(const Piece& source, const Piece& destination) {
    return !destination.isEmpty() && source.color != destination.color;
}

bool isSameColor(const Piece& source, const Piece& destination) {
    return !destination.isEmpty() && source.color == destination.color;
}
} // namespace

bool MoveValidator::isLegalMove(const Board& board, const Move& move) {
    if (!board.isInBounds(move.from) || !board.isInBounds(move.to) || move.from == move.to) {
        return false;
    }

    const Piece source = board.pieceAt(move.from);
    const Piece destination = board.pieceAt(move.to);

    if (source.isEmpty()) {
        return false;
    }

    if (source.color != board.currentTurn()) {
        return false;
    }

    if (isSameColor(source, destination)) {
        return false;
    }

    const int rowDelta = move.to.row - move.from.row;
    const int colDelta = move.to.col - move.from.col;
    const int absRow = std::abs(rowDelta);
    const int absCol = std::abs(colDelta);

    switch (source.type) {
        case PieceType::Pawn: {
            const int direction = (source.color == Color::White) ? -1 : 1;
            const int startRow = (source.color == Color::White) ? 6 : 1;

            if (colDelta == 0 && destination.isEmpty()) {
                if (rowDelta == direction) {
                    return true;
                }
                if (move.from.row == startRow && rowDelta == 2 * direction) {
                    const Position middle {move.from.row + direction, move.from.col};
                    return board.pieceAt(middle).isEmpty();
                }
            }

            if (absCol == 1 && rowDelta == direction && isOpponent(source, destination)) {
                return true;
            }

            if (absCol == 1 && rowDelta == direction && destination.isEmpty()
                && board.enPassantTarget().has_value() && board.enPassantTarget().value() == move.to) {
                return true;
            }

            return false;
        }
        case PieceType::Knight:
            return (absRow == 2 && absCol == 1) || (absRow == 1 && absCol == 2);
        case PieceType::Bishop:
            return absRow == absCol && board.isPathClear(move.from, move.to);
        case PieceType::Rook:
            return (rowDelta == 0 || colDelta == 0) && board.isPathClear(move.from, move.to);
        case PieceType::Queen:
            return ((absRow == absCol) || (rowDelta == 0 || colDelta == 0)) && board.isPathClear(move.from, move.to);
        case PieceType::King: {
            if (absRow <= 1 && absCol <= 1) {
                return true;
            }

            if (rowDelta != 0 || absCol != 2) {
                return false;
            }

            if (source.color == Color::White) {
                if (move.to == Position {7, 6}) {
                    return board.canCastleKingside(Color::White)
                        && !board.isInCheck(Color::White)
                        && !board.isSquareAttacked({7, 5}, Color::Black)
                        && !board.isSquareAttacked({7, 6}, Color::Black);
                }
                if (move.to == Position {7, 2}) {
                    return board.canCastleQueenside(Color::White)
                        && !board.isInCheck(Color::White)
                        && !board.isSquareAttacked({7, 3}, Color::Black)
                        && !board.isSquareAttacked({7, 2}, Color::Black);
                }
            }

            if (source.color == Color::Black) {
                if (move.to == Position {0, 6}) {
                    return board.canCastleKingside(Color::Black)
                        && !board.isInCheck(Color::Black)
                        && !board.isSquareAttacked({0, 5}, Color::White)
                        && !board.isSquareAttacked({0, 6}, Color::White);
                }
                if (move.to == Position {0, 2}) {
                    return board.canCastleQueenside(Color::Black)
                        && !board.isInCheck(Color::Black)
                        && !board.isSquareAttacked({0, 3}, Color::White)
                        && !board.isSquareAttacked({0, 2}, Color::White);
                }
            }

            return false;
        }
        case PieceType::None:
            return false;
    }

    return false;
}

} // namespace chess
