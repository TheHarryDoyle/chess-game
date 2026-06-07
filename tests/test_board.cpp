#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>

#include "chess/Board.h"
#include "chess/ChessEngine.h"

using chess::Board;
using chess::ChessEngine;
using chess::Color;
using chess::Move;
using chess::Piece;
using chess::PieceType;
using chess::Position;

namespace {
void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void placeKings(Board& board) {
    board.setPiece({7, 4}, Piece {PieceType::King, Color::White});
    board.setPiece({0, 4}, Piece {PieceType::King, Color::Black});
}

void testInitialBoardSetup() {
    Board board;

    expect(board.pieceAt({7, 4}).type == PieceType::King, "White king should start on e1.");
    expect(board.pieceAt({7, 4}).color == Color::White, "White king should be white.");
    expect(board.pieceAt({0, 4}).type == PieceType::King, "Black king should start on e8.");
    expect(board.pieceAt({0, 4}).color == Color::Black, "Black king should be black.");
    expect(board.pieceAt({6, 0}).type == PieceType::Pawn, "White pawn should start on row 6.");
    expect(board.pieceAt({1, 7}).type == PieceType::Pawn, "Black pawn should start on row 1.");
}

void testWhitePawnSingleAdvance() {
    Board board;
    expect(board.movePiece({{6, 4}, {5, 4}}), "White pawn should move one square forward.");
    expect(board.pieceAt({5, 4}).type == PieceType::Pawn, "White pawn should occupy new square.");
    expect(board.currentTurn() == Color::Black, "Turn should switch after a valid move.");
}

void testKnightCanJump() {
    Board board;
    expect(board.movePiece({{7, 6}, {5, 5}}), "Knight should be able to jump over pieces.");
}

void testBishopBlockedAtStart() {
    Board board;
    expect(!board.movePiece({{7, 2}, {5, 4}}), "Bishop should not move through a blocking pawn.");
}

void testWrongTurnIllegal() {
    Board board;
    expect(!board.movePiece({{1, 4}, {2, 4}}), "Black should not move first.");
}

void testCannotCaptureOwnPiece() {
    Board board;
    board.clear();
    placeKings(board);
    board.setPiece({4, 4}, Piece {PieceType::Rook, Color::White});
    board.setPiece({4, 7}, Piece {PieceType::Knight, Color::White});
    board.setCurrentTurn(Color::White);

    expect(!board.movePiece({{4, 4}, {4, 7}}), "A piece should not capture a piece of the same color.");
}

void testKingsideCastling() {
    Board board;
    board.clear();
    board.setPiece({7, 4}, Piece {PieceType::King, Color::White});
    board.setPiece({7, 7}, Piece {PieceType::Rook, Color::White});
    board.setPiece({0, 4}, Piece {PieceType::King, Color::Black});
    board.setCurrentTurn(Color::White);

    expect(board.movePiece({{7, 4}, {7, 6}}), "White should be able to castle kingside.");
    expect(board.pieceAt({7, 6}).type == PieceType::King, "King should land on g1 after castling.");
    expect(board.pieceAt({7, 5}).type == PieceType::Rook, "Rook should land on f1 after castling.");
}

void testEnPassant() {
    Board board;
    board.clear();
    board.setPiece({7, 4}, Piece {PieceType::King, Color::White});
    board.setPiece({0, 4}, Piece {PieceType::King, Color::Black});
    board.setPiece({3, 4}, Piece {PieceType::Pawn, Color::White});
    board.setPiece({1, 5}, Piece {PieceType::Pawn, Color::Black});
    board.setCurrentTurn(Color::Black);

    expect(board.movePiece({{1, 5}, {3, 5}}), "Black pawn should advance two squares.");
    expect(board.movePiece({{3, 4}, {2, 5}}), "White should capture en passant immediately.");
    expect(board.pieceAt({3, 5}).isEmpty(), "Captured pawn should be removed during en passant.");
    expect(board.pieceAt({2, 5}).type == PieceType::Pawn, "White pawn should occupy the en passant target square.");
}

void testPromotionAutoQueens() {
    Board board;
    board.clear();
    board.setPiece({7, 4}, Piece {PieceType::King, Color::White});
    board.setPiece({0, 4}, Piece {PieceType::King, Color::Black});
    board.setPiece({1, 0}, Piece {PieceType::Pawn, Color::White});
    board.setCurrentTurn(Color::White);

    expect(board.movePiece({{1, 0}, {0, 0}}), "White pawn should reach promotion square.");
    expect(board.pieceAt({0, 0}).type == PieceType::Queen, "Promoted pawn should become a queen.");
}

void testCheckDetection() {
    Board board;
    board.clear();
    board.setPiece({7, 4}, Piece {PieceType::King, Color::White});
    board.setPiece({0, 4}, Piece {PieceType::King, Color::Black});
    board.setPiece({7, 0}, Piece {PieceType::Rook, Color::Black});

    expect(board.isInCheck(Color::White), "White king should be in check from the rook.");
}

void testSelfCheckRejection() {
    Board board;
    board.clear();
    board.setPiece({7, 4}, Piece {PieceType::King, Color::White});
    board.setPiece({7, 3}, Piece {PieceType::Rook, Color::White});
    board.setPiece({7, 0}, Piece {PieceType::Rook, Color::Black});
    board.setPiece({0, 4}, Piece {PieceType::King, Color::Black});
    board.setCurrentTurn(Color::White);

    expect(!board.movePiece({{7, 3}, {6, 3}}), "Move exposing the king to check should be rejected.");
}

void testCheckmateDetectionWithFoolsMate() {
    Board board;

    expect(board.movePiece({{6, 5}, {5, 5}}), "1. f3 should be legal.");
    expect(board.movePiece({{1, 4}, {3, 4}}), "... e5 should be legal.");
    expect(board.movePiece({{6, 6}, {4, 6}}), "2. g4 should be legal.");
    expect(board.movePiece({{0, 3}, {4, 7}}), "... Qh4# should be legal.");
    expect(board.isCheckmate(Color::White), "Fool's mate position should be recognized as checkmate.");
}

void testStalemateDetection() {
    Board board;
    board.clear();
    board.setPiece({0, 0}, Piece {PieceType::King, Color::Black});
    board.setPiece({2, 2}, Piece {PieceType::King, Color::White});
    board.setPiece({1, 2}, Piece {PieceType::Queen, Color::White});

    expect(board.isStalemate(Color::Black), "Known stalemate position should be detected.");
}

void testEngineReturnsLegalMove() {
    Board board;
    ChessEngine engine(2);

    const auto move = engine.chooseMove(board, Color::White);
    expect(move.has_value(), "Engine should return a move from the starting position.");

    Board verification = board;
    verification.setCurrentTurn(Color::White);
    expect(verification.movePiece(move.value()), "Engine move should be legal when applied to the board.");
}

void testEngineReturnsNoMoveInCheckmate() {
    Board board;
    expect(board.movePiece({{6, 5}, {5, 5}}), "1. f3 should be legal for engine mate test.");
    expect(board.movePiece({{1, 4}, {3, 4}}), "... e5 should be legal for engine mate test.");
    expect(board.movePiece({{6, 6}, {4, 6}}), "2. g4 should be legal for engine mate test.");
    expect(board.movePiece({{0, 3}, {4, 7}}), "... Qh4# should be legal for engine mate test.");

    ChessEngine engine(2);
    const auto move = engine.chooseMove(board, Color::White);
    expect(!move.has_value(), "Engine should return no move when the side to move is already checkmated.");
}

} // namespace

int main() {
    try {
        testInitialBoardSetup();
        testWhitePawnSingleAdvance();
        testKnightCanJump();
        testBishopBlockedAtStart();
        testWrongTurnIllegal();
        testCannotCaptureOwnPiece();
        testKingsideCastling();
        testEnPassant();
        testPromotionAutoQueens();
        testCheckDetection();
        testSelfCheckRejection();
        testCheckmateDetectionWithFoolsMate();
        testStalemateDetection();
        testEngineReturnsLegalMove();
        testEngineReturnsNoMoveInCheckmate();
        std::cout << "All Phase II tests passed.\n";
        return EXIT_SUCCESS;
    } catch (const std::exception& ex) {
        std::cerr << "Test failure: " << ex.what() << '\n';
        return EXIT_FAILURE;
    }
}
