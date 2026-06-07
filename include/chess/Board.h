#pragma once

#include <array>
#include <optional>
#include <string>
#include <vector>

#include "Move.h"
#include "Piece.h"

namespace chess {

class Board {
public:
    Board();

    void reset();
    void clear();

    [[nodiscard]] Piece pieceAt(const Position& position) const;
    void setPiece(const Position& position, const Piece& piece);

    [[nodiscard]] bool isPathClear(const Position& from, const Position& to) const;
    [[nodiscard]] bool movePiece(const Move& move);
    [[nodiscard]] bool isInBounds(const Position& position) const;

    [[nodiscard]] Color currentTurn() const;
    void setCurrentTurn(Color color);
    void toggleTurn();

    [[nodiscard]] bool canCastleKingside(Color color) const;
    [[nodiscard]] bool canCastleQueenside(Color color) const;
    [[nodiscard]] std::optional<Position> enPassantTarget() const;

    [[nodiscard]] bool isSquareAttacked(const Position& square, Color byColor) const;
    [[nodiscard]] bool isInCheck(Color color) const;
    [[nodiscard]] std::vector<Move> generateLegalMoves(Color color) const;
    [[nodiscard]] bool hasAnyLegalMove(Color color) const;
    [[nodiscard]] bool isCheckmate(Color color) const;
    [[nodiscard]] bool isStalemate(Color color) const;

    [[nodiscard]] std::string toAscii() const;

private:
    [[nodiscard]] std::optional<Position> findKing(Color color) const;
    void applyMoveUnchecked(const Move& move);
    void markRookMoved(const Position& position);

    std::array<std::array<Piece, 8>, 8> squares_ {};
    Color currentTurn_ {Color::White};
    std::optional<Position> enPassantTarget_ {};
    bool whiteKingMoved_ {false};
    bool blackKingMoved_ {false};
    bool whiteKingsideRookMoved_ {false};
    bool whiteQueensideRookMoved_ {false};
    bool blackKingsideRookMoved_ {false};
    bool blackQueensideRookMoved_ {false};
};

} // namespace chess
