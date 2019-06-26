#include "../include/chess/engine/piece.hpp"
#include <cctype>
#include <stdexcept>

using namespace chess;

Piece::Piece() {
	color = PieceColor::White;
	type = PieceType::None;
}

Piece::Piece(char piece) {
	color = isupper(piece) ? PieceColor::White : PieceColor::Black;
	switch(toupper(piece)) {
	case 'K':
		type = PieceType::King;
		break;
	case 'Q':
		type = PieceType::Queen;
		break;
	case 'R':
		type = PieceType::Rook;
		break;
	case 'B':
		type = PieceType::Bishop;
		break;
	case 'N':
		type = PieceType::Knight;
		break;
	case 'P':
		type = PieceType::Pawn;
		break;
	default:
		throw std::invalid_argument("Error: Invalid character in Piece constructor");
	}
}

bool Piece::operator!=(const Piece other) {
    if(type != other.type) {
        return true;
    }
    if(color != other.color && type != PieceType::None) {
        return true;
    }
    return false;
}

bool Piece::operator==(const Piece other) {
    return !(*this != other);
}

Piece::operator char() {
    char c;
    switch(type) {
    case PieceType::King:
        c = 'K';
        break;
    case PieceType::Queen:
        c = 'Q';
        break;
    case PieceType::Rook:
        c =  'R';
        break;
    case PieceType::Bishop:
        c =  'B';
        break;
    case PieceType::Knight:
        c =  'N';
        break;
    case PieceType::Pawn:
        c =  'P';
        break;
    default:
        c =  ' ';
        break;
    }
    if(color == PieceColor::White) {
        return toupper(c);
    } else {
        return tolower(c);
    }
}

size_t Piece::colorIndex(PieceColor color) {
   if(color == PieceColor::White) {
       return 0;
   } else {
       return 1;
   }
}
