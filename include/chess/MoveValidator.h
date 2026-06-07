#pragma once

#include "Board.h"
#include "Move.h"

namespace chess {

class MoveValidator {
public:
    [[nodiscard]] static bool isLegalMove(const Board& board, const Move& move);
};

} // namespace chess
