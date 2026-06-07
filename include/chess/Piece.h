#pragma once

#include <string>

namespace chess {

enum class Color {
    None,
    White,
    Black
};

enum class PieceType {
    None,
    Pawn,
    Knight,
    Bishop,
    Rook,
    Queen,
    King
};

struct Piece {
    PieceType type {PieceType::None};
    Color color {Color::None};

    [[nodiscard]] bool isEmpty() const {
        return type == PieceType::None || color == Color::None;
    }
};

inline Color oppositeColor(Color color) {
    if (color == Color::White) {
        return Color::Black;
    }
    if (color == Color::Black) {
        return Color::White;
    }
    return Color::None;
}

inline std::string pieceToUnicode(const Piece& piece) {
    if (piece.isEmpty()) {
        return "";
    }

    if (piece.color == Color::White) {
        switch (piece.type) {
            case PieceType::Pawn: return u8"♙";
            case PieceType::Knight: return u8"♘";
            case PieceType::Bishop: return u8"♗";
            case PieceType::Rook: return u8"\u2656";
            case PieceType::Queen: return u8"♕";
            case PieceType::King: return u8"♔";
            case PieceType::None: return "";
        }
    }

    switch (piece.type) {
        case PieceType::Pawn: return u8"♟";
        case PieceType::Knight: return u8"♞";
        case PieceType::Bishop: return u8"♝";
        case PieceType::Rook: return u8"♜";
        case PieceType::Queen: return u8"♛";
        case PieceType::King: return u8"♚";
        case PieceType::None: return "";
    }

    return "";
}

} // namespace chess
