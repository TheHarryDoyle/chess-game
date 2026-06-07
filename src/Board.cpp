#include "chess/Board.h"

#include <cmath>
#include <sstream>

#include "chess/MoveValidator.h"

namespace chess {

namespace {
Piece makePiece(PieceType type, Color color) {
    return Piece {type, color};
}

bool isStartingRook(const Piece& piece, Color color) {
    return piece.type == PieceType::Rook && piece.color == color;
}
} // namespace

Board::Board() {
    reset();
}

void Board::clear() {
    for (auto& row : squares_) {
        row.fill(Piece {});
    }

    currentTurn_ = Color::White;
    enPassantTarget_.reset();
    whiteKingMoved_ = false;
    blackKingMoved_ = false;
    whiteKingsideRookMoved_ = false;
    whiteQueensideRookMoved_ = false;
    blackKingsideRookMoved_ = false;
    blackQueensideRookMoved_ = false;
}

void Board::reset() {
    clear();

    squares_[0] = {makePiece(PieceType::Rook, Color::Black),
                   makePiece(PieceType::Knight, Color::Black),
                   makePiece(PieceType::Bishop, Color::Black),
                   makePiece(PieceType::Queen, Color::Black),
                   makePiece(PieceType::King, Color::Black),
                   makePiece(PieceType::Bishop, Color::Black),
                   makePiece(PieceType::Knight, Color::Black),
                   makePiece(PieceType::Rook, Color::Black)};

    squares_[1].fill(makePiece(PieceType::Pawn, Color::Black));
    squares_[6].fill(makePiece(PieceType::Pawn, Color::White));

    squares_[7] = {makePiece(PieceType::Rook, Color::White),
                   makePiece(PieceType::Knight, Color::White),
                   makePiece(PieceType::Bishop, Color::White),
                   makePiece(PieceType::Queen, Color::White),
                   makePiece(PieceType::King, Color::White),
                   makePiece(PieceType::Bishop, Color::White),
                   makePiece(PieceType::Knight, Color::White),
                   makePiece(PieceType::Rook, Color::White)};
}

Piece Board::pieceAt(const Position& position) const {
    if (!isInBounds(position)) {
        return Piece {};
    }
    return squares_[position.row][position.col];
}

void Board::setPiece(const Position& position, const Piece& piece) {
    if (!isInBounds(position)) {
        return;
    }
    squares_[position.row][position.col] = piece;
}

bool Board::isPathClear(const Position& from, const Position& to) const {
    const int rowDelta = to.row - from.row;
    const int colDelta = to.col - from.col;

    const int rowStep = (rowDelta == 0) ? 0 : (rowDelta > 0 ? 1 : -1);
    const int colStep = (colDelta == 0) ? 0 : (colDelta > 0 ? 1 : -1);

    int currentRow = from.row + rowStep;
    int currentCol = from.col + colStep;

    while (currentRow != to.row || currentCol != to.col) {
        if (!pieceAt({currentRow, currentCol}).isEmpty()) {
            return false;
        }
        currentRow += rowStep;
        currentCol += colStep;
    }

    return true;
}

bool Board::movePiece(const Move& move) {
    if (!MoveValidator::isLegalMove(*this, move)) {
        return false;
    }

    Board simulated = *this;
    simulated.applyMoveUnchecked(move);

    const Piece movingPiece = pieceAt(move.from);
    if (simulated.isInCheck(movingPiece.color)) {
        return false;
    }

    applyMoveUnchecked(move);
    return true;
}

bool Board::isInBounds(const Position& position) const {
    return position.isValid();
}

Color Board::currentTurn() const {
    return currentTurn_;
}

void Board::setCurrentTurn(Color color) {
    currentTurn_ = color;
}

void Board::toggleTurn() {
    currentTurn_ = oppositeColor(currentTurn_);
}

bool Board::canCastleKingside(Color color) const {
    if (color == Color::White) {
        return !whiteKingMoved_
            && !whiteKingsideRookMoved_
            && pieceAt({7, 4}).type == PieceType::King
            && pieceAt({7, 4}).color == Color::White
            && isStartingRook(pieceAt({7, 7}), Color::White)
            && pieceAt({7, 5}).isEmpty()
            && pieceAt({7, 6}).isEmpty();
    }

    if (color == Color::Black) {
        return !blackKingMoved_
            && !blackKingsideRookMoved_
            && pieceAt({0, 4}).type == PieceType::King
            && pieceAt({0, 4}).color == Color::Black
            && isStartingRook(pieceAt({0, 7}), Color::Black)
            && pieceAt({0, 5}).isEmpty()
            && pieceAt({0, 6}).isEmpty();
    }

    return false;
}

bool Board::canCastleQueenside(Color color) const {
    if (color == Color::White) {
        return !whiteKingMoved_
            && !whiteQueensideRookMoved_
            && pieceAt({7, 4}).type == PieceType::King
            && pieceAt({7, 4}).color == Color::White
            && isStartingRook(pieceAt({7, 0}), Color::White)
            && pieceAt({7, 1}).isEmpty()
            && pieceAt({7, 2}).isEmpty()
            && pieceAt({7, 3}).isEmpty();
    }

    if (color == Color::Black) {
        return !blackKingMoved_
            && !blackQueensideRookMoved_
            && pieceAt({0, 4}).type == PieceType::King
            && pieceAt({0, 4}).color == Color::Black
            && isStartingRook(pieceAt({0, 0}), Color::Black)
            && pieceAt({0, 1}).isEmpty()
            && pieceAt({0, 2}).isEmpty()
            && pieceAt({0, 3}).isEmpty();
    }

    return false;
}

std::optional<Position> Board::enPassantTarget() const {
    return enPassantTarget_;
}

bool Board::isSquareAttacked(const Position& square, Color byColor) const {
    if (!square.isValid() || byColor == Color::None) {
        return false;
    }

    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            const Position from {row, col};
            const Piece attacker = pieceAt(from);
            if (attacker.isEmpty() || attacker.color != byColor) {
                continue;
            }

            const int rowDelta = square.row - from.row;
            const int colDelta = square.col - from.col;
            const int absRow = std::abs(rowDelta);
            const int absCol = std::abs(colDelta);

            switch (attacker.type) {
                case PieceType::Pawn: {
                    const int direction = (byColor == Color::White) ? -1 : 1;
                    if (rowDelta == direction && absCol == 1) {
                        return true;
                    }
                    break;
                }
                case PieceType::Knight:
                    if ((absRow == 2 && absCol == 1) || (absRow == 1 && absCol == 2)) {
                        return true;
                    }
                    break;
                case PieceType::Bishop:
                    if (absRow == absCol && isPathClear(from, square)) {
                        return true;
                    }
                    break;
                case PieceType::Rook:
                    if ((rowDelta == 0 || colDelta == 0) && isPathClear(from, square)) {
                        return true;
                    }
                    break;
                case PieceType::Queen:
                    if (((absRow == absCol) || (rowDelta == 0 || colDelta == 0)) && isPathClear(from, square)) {
                        return true;
                    }
                    break;
                case PieceType::King:
                    if (absRow <= 1 && absCol <= 1) {
                        return true;
                    }
                    break;
                case PieceType::None:
                    break;
            }
        }
    }

    return false;
}

std::optional<Position> Board::findKing(Color color) const {
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            const Piece piece = pieceAt({row, col});
            if (piece.type == PieceType::King && piece.color == color) {
                return Position {row, col};
            }
        }
    }
    return std::nullopt;
}

bool Board::isInCheck(Color color) const {
    const auto kingPosition = findKing(color);
    if (!kingPosition.has_value()) {
        return false;
    }
    return isSquareAttacked(kingPosition.value(), oppositeColor(color));
}

std::vector<Move> Board::generateLegalMoves(Color color) const {
    std::vector<Move> legalMoves;

    for (int fromRow = 0; fromRow < 8; ++fromRow) {
        for (int fromCol = 0; fromCol < 8; ++fromCol) {
            const Position from {fromRow, fromCol};
            const Piece source = pieceAt(from);
            if (source.isEmpty() || source.color != color) {
                continue;
            }

            for (int toRow = 0; toRow < 8; ++toRow) {
                for (int toCol = 0; toCol < 8; ++toCol) {
                    Board candidate = *this;
                    candidate.setCurrentTurn(color);
                    const Move move {{fromRow, fromCol}, {toRow, toCol}};
                    if (candidate.movePiece(move)) {
                        legalMoves.push_back(move);
                    }
                }
            }
        }
    }

    return legalMoves;
}

bool Board::hasAnyLegalMove(Color color) const {
    return !generateLegalMoves(color).empty();
}

bool Board::isCheckmate(Color color) const {
    return isInCheck(color) && !hasAnyLegalMove(color);
}

bool Board::isStalemate(Color color) const {
    return !isInCheck(color) && !hasAnyLegalMove(color);
}

void Board::markRookMoved(const Position& position) {
    if (position == Position {7, 0}) {
        whiteQueensideRookMoved_ = true;
    } else if (position == Position {7, 7}) {
        whiteKingsideRookMoved_ = true;
    } else if (position == Position {0, 0}) {
        blackQueensideRookMoved_ = true;
    } else if (position == Position {0, 7}) {
        blackKingsideRookMoved_ = true;
    }
}

void Board::applyMoveUnchecked(const Move& move) {
    const Piece movingPiece = pieceAt(move.from);
    const Piece destinationPiece = pieceAt(move.to);
    const int rowDelta = move.to.row - move.from.row;
    const int colDelta = move.to.col - move.from.col;

    if (destinationPiece.type == PieceType::Rook) {
        markRookMoved(move.to);
    }

    if (movingPiece.type == PieceType::King) {
        if (movingPiece.color == Color::White) {
            whiteKingMoved_ = true;
        } else if (movingPiece.color == Color::Black) {
            blackKingMoved_ = true;
        }

        if (std::abs(colDelta) == 2) {
            if (colDelta > 0) {
                const Position rookFrom {move.from.row, 7};
                const Position rookTo {move.from.row, 5};
                setPiece(rookTo, pieceAt(rookFrom));
                setPiece(rookFrom, Piece {});
                markRookMoved(rookFrom);
            } else {
                const Position rookFrom {move.from.row, 0};
                const Position rookTo {move.from.row, 3};
                setPiece(rookTo, pieceAt(rookFrom));
                setPiece(rookFrom, Piece {});
                markRookMoved(rookFrom);
            }
        }
    }

    if (movingPiece.type == PieceType::Rook) {
        markRookMoved(move.from);
    }

    if (movingPiece.type == PieceType::Pawn && move.from.col != move.to.col && destinationPiece.isEmpty()
        && enPassantTarget_.has_value() && enPassantTarget_.value() == move.to) {
        setPiece({move.from.row, move.to.col}, Piece {});
    }

    setPiece(move.to, movingPiece);
    setPiece(move.from, Piece {});

    if (movingPiece.type == PieceType::Pawn && (move.to.row == 0 || move.to.row == 7)) {
        setPiece(move.to, Piece {PieceType::Queen, movingPiece.color});
    }

    if (movingPiece.type == PieceType::Pawn && std::abs(rowDelta) == 2) {
        enPassantTarget_ = Position {move.from.row + (rowDelta / 2), move.from.col};
    } else {
        enPassantTarget_.reset();
    }

    toggleTurn();
}

std::string Board::toAscii() const {
    std::ostringstream output;
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            const Piece piece = squares_[row][col];
            output << (piece.isEmpty() ? "." : pieceToUnicode(piece));
            if (col < 7) {
                output << ' ';
            }
        }
        output << '\n';
    }
    return output.str();
}

} // namespace chess
