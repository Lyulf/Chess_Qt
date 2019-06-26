#include "../include/chess/engine/pressure_factory.hpp"
#define _USE_MATH_DEFINES
#include <math.h>

using namespace chess;

PressureFactory::PressureFactory(Board board, const std::pair<short, short>& piecePosition)
	: _board(board), _selectedSquare(_board[piecePosition]), _moves() {
	generateMoves();
}

bool PressureFactory::canAttack(const std::pair<short, short> destination) {
	return _moves.find(destination) != _moves.end();
}

std::unordered_map<std::pair<short, short>, MoveType, PairHash> PressureFactory::getMoves() {
	return _moves;
}

void PressureFactory::generateMoves() {
	_moves.clear();
	switch(_selectedSquare.piece.type) {
	case PieceType::King:
		generateVerticalMoves();
		generateHorizontalMoves();
		generateDiagonalMoves();
		break;
	case PieceType::Queen:
		generateVerticalMoves(true);
		generateHorizontalMoves(true);
		generateDiagonalMoves(true);
		break;
	case PieceType::Rook:
		generateHorizontalMoves(true);
		generateVerticalMoves(true);
		break;
	case PieceType::Bishop:
		generateDiagonalMoves(true);
		break;
	case PieceType::Knight:
		generateJumpMoves();
		break;
	case PieceType::Pawn: {
		short x = _selectedSquare.getX();
		short y = _selectedSquare.getY();
		short step = _selectedSquare.piece.color == PieceColor::White ? 1 : -1;
        MoveType moveType = (y + step) == 0 || (y + step) == 7 ? MoveType::Promotion : MoveType::Normal;
        addMove(x + 1, y + step, moveType);
        addMove(x - 1, y + step, moveType);
		break;
	}
	default:
		return;
	}
}

void PressureFactory::generateVerticalMoves(bool multiMove) {
	short _x = _selectedSquare.getX();
	short _y = _selectedSquare.getY();
	short minDistanceY, maxDistanceY;
	if(multiMove) {
		minDistanceY = 0;
		maxDistanceY = 7;
	} else {
		minDistanceY = _y - 1;
		maxDistanceY = _y + 1;
		minDistanceY = minDistanceY > 0 ? minDistanceY : 0;
		maxDistanceY = maxDistanceY < 7 ? maxDistanceY : 7;
	}
	for(short y = _y + 1; y <= maxDistanceY; y++) {
		if(!addMove(_x, y)) {
			break;
		}
	}
	for(short y = _y - 1; y >= minDistanceY; y--) {
		if(!addMove(_x, y)) {
			break;
		}
	}
}
void PressureFactory::generateHorizontalMoves(bool multiMove) {
	short _x = _selectedSquare.getX();
	short _y = _selectedSquare.getY();
	short minDistanceX, maxDistanceX;
	if(multiMove) {
		minDistanceX = 0;
		maxDistanceX = 7;
	} else {
		minDistanceX = _x - 1;
		maxDistanceX = _x + 1;
		minDistanceX = minDistanceX > 0 ? minDistanceX : 0;
		maxDistanceX = maxDistanceX < 7 ? maxDistanceX : 7;
	}
	for(short x = _x + 1; x <= maxDistanceX; x++) {
		if(!addMove(x, _y)) {
			break;
		}
	}
	for(short x = _x - 1; x >= minDistanceX; x--) {
		if(!addMove(x, _y)) {
			break;
		}
	}
}
void PressureFactory::generateDiagonalMoves(bool multiMove) {
	short _x = _selectedSquare.getX();
	short _y = _selectedSquare.getY();
	short x, y;
	short minDistanceX, minDistanceY;
	short maxDistanceX, maxDistanceY;
	if(multiMove) {
		minDistanceX = minDistanceY = 0;
		maxDistanceX = maxDistanceY = 7;
	} else {
		minDistanceX = _x - 1;
		minDistanceY = _y - 1;
		maxDistanceX = _x + 1;
		maxDistanceY = _y + 1;
		minDistanceX = minDistanceX > 0 ? minDistanceX : 0;
		minDistanceY = minDistanceY > 0 ? minDistanceY : 0;
		maxDistanceX = maxDistanceX < 7 ? maxDistanceX : 7;
		maxDistanceY = maxDistanceY < 7 ? maxDistanceY : 7;
	}
	for(x = _x + 1, y = _y + 1; x <= maxDistanceX && y <= maxDistanceY; x++, y++) {
		if(!addMove(x, y)) {
		break;
		}
	}
	for(x = _x + 1, y = _y - 1; x <= maxDistanceX && y >= minDistanceY; x++, y--) {
		if(!addMove(x, y)) {
		break;
		}
	}
	for(x = _x - 1, y = _y - 1; x >= minDistanceX && y >= minDistanceY; x--, y--) {
	if(!addMove(x, y)) {
			break;
		}
	}
	for(x = _x - 1, y = _y + 1; x >= minDistanceX && y <= maxDistanceY; x--, y++) {
		if(!addMove(x, y)) {
			break;
		}
	}
}

void PressureFactory::generateJumpMoves() {
	short _x = _selectedSquare.getX();
	short _y = _selectedSquare.getY();
	for(short i = 0; i < 4; i++) {
		for(short j = -1; j <= 1; j += 2) {
			short x = static_cast<short>(2 * sin(i * M_PI_2)) + _x;
			short y = static_cast<short>(2 * cos(i * M_PI_2)) + _y;
			if(i % 2) {
				y += j;
			} else {
				x += j;
			}
			addMove(x, y);
		}
	}
}

bool PressureFactory::addMove(short x, short y, MoveType moveType) {
	return addMove(std::make_pair(x, y), moveType);
}

bool PressureFactory::addMove(const std::pair<short, short>& destination, MoveType moveType) {
	if(!Board::positionExists(destination)) {
		return false;
	}
	Piece pieceAtDestination = _board[destination].piece;
	if(pieceAtDestination.type != PieceType::None) {
		if(isEnemy(destination)) {
			_moves.insert(std::make_pair(destination, moveType));
		}
		return false;
	} else {
		_moves.insert(std::make_pair(destination, moveType));
		return true;
	}
}

bool PressureFactory::isEnemy(short x, short y) {
	return isEnemy(std::make_pair(x, y));
}
bool PressureFactory::isEnemy(const std::pair<short, short>& position) {
	if(!Board::positionExists(position)) {
		return false;
	}
	Piece piece = _board[position].piece;
	if(piece.type != PieceType::None) {
		return _selectedSquare.piece.color != piece.color;
	} else {
		return false;
	}
}
