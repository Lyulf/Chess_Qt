#pragma once

namespace chess {

enum class PieceType{
	None,
	Pawn,
	Knight,
	Bishop,
	Rook,
	Queen,
	King
};

enum class PieceColor {
	White,
	Black
};

struct Piece {
	PieceType type;
	PieceColor color;

	Piece();
    Piece(char piece);

    bool operator!=(const Piece other);
    bool operator==(const Piece other);

    operator char();

    static size_t colorIndex(PieceColor color);
};

}
