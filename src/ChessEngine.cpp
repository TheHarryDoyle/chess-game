#include "chess/ChessEngine.h"

#include <algorithm>
#include <limits>

#include "chess/Piece.h"

namespace chess {

namespace {
int pieceValue(PieceType type) {
    switch (type) {
        case PieceType::Pawn: return 100;
        case PieceType::Knight: return 320;
        case PieceType::Bishop: return 330;
        case PieceType::Rook: return 500;
        case PieceType::Queen: return 900;
        case PieceType::King: return 20000;
        case PieceType::None: return 0;
    }
    return 0;
}
} // namespace

ChessEngine::ChessEngine(int maxDepth)
    : maxDepth_(std::max(1, maxDepth)) {
}

std::optional<Move> ChessEngine::chooseMove(const Board& board, Color color) const {
    Board root = board;
    root.setCurrentTurn(color);
    const auto moves = root.generateLegalMoves(color);
    if (moves.empty()) {
        return std::nullopt;
    }

    int bestScore = std::numeric_limits<int>::min();
    Move bestMove = moves.front();

    for (const auto& move : moves) {
        Board child = root;
        child.setCurrentTurn(color);
        child.movePiece(move);

        const int score = minimax(
            child,
            maxDepth_ - 1,
            std::numeric_limits<int>::min(),
            std::numeric_limits<int>::max(),
            color,
            oppositeColor(color)
        );

        if (score > bestScore) {
            bestScore = score;
            bestMove = move;
        }
    }

    return bestMove;
}

int ChessEngine::minimax(Board board, int depth, int alpha, int beta, Color maximizingColor, Color sideToMove) const {
    board.setCurrentTurn(sideToMove);

    if (board.isCheckmate(sideToMove)) {
        return (sideToMove == maximizingColor) ? -100000 : 100000;
    }
    if (board.isStalemate(sideToMove) || depth == 0) {
        return evaluate(board, maximizingColor);
    }

    const auto moves = board.generateLegalMoves(sideToMove);
    if (moves.empty()) {
        return evaluate(board, maximizingColor);
    }

    if (sideToMove == maximizingColor) {
        int bestValue = std::numeric_limits<int>::min();
        for (const auto& move : moves) {
            Board child = board;
            child.setCurrentTurn(sideToMove);
            child.movePiece(move);
            bestValue = std::max(bestValue, minimax(child, depth - 1, alpha, beta, maximizingColor, oppositeColor(sideToMove)));
            alpha = std::max(alpha, bestValue);
            if (beta <= alpha) {
                break;
            }
        }
        return bestValue;
    }

    int bestValue = std::numeric_limits<int>::max();
    for (const auto& move : moves) {
        Board child = board;
        child.setCurrentTurn(sideToMove);
        child.movePiece(move);
        bestValue = std::min(bestValue, minimax(child, depth - 1, alpha, beta, maximizingColor, oppositeColor(sideToMove)));
        beta = std::min(beta, bestValue);
        if (beta <= alpha) {
            break;
        }
    }
    return bestValue;
}

int ChessEngine::evaluate(const Board& board, Color perspective) const {
    int score = 0;
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            const Piece piece = board.pieceAt({row, col});
            if (piece.isEmpty()) {
                continue;
            }
            const int value = pieceValue(piece.type);
            score += (piece.color == Color::White) ? value : -value;
        }
    }

    return (perspective == Color::White) ? score : -score;
}

} // namespace chess
