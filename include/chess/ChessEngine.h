#pragma once

#include <optional>

#include "Board.h"
#include "Move.h"

namespace chess {

class ChessEngine {
public:
    explicit ChessEngine(int maxDepth = 2);

    [[nodiscard]] std::optional<Move> chooseMove(const Board& board, Color color) const;

private:
    [[nodiscard]] int minimax(Board board, int depth, int alpha, int beta, Color maximizingColor, Color sideToMove) const;
    [[nodiscard]] int evaluate(const Board& board, Color perspective) const;

    int maxDepth_ {2};
};

} // namespace chess
