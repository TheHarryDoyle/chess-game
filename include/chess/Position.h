#pragma once

namespace chess {

struct Position {
    int row {};
    int col {};

    [[nodiscard]] bool isValid() const {
        return row >= 0 && row < 8 && col >= 0 && col < 8;
    }

    [[nodiscard]] bool operator==(const Position& other) const {
        return row == other.row && col == other.col;
    }
};

} // namespace chess
