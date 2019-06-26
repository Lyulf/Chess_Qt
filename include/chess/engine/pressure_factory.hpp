#pragma once

#include "board.hpp"
#include "move_type.hpp"

namespace chess {

class PressureFactory {
public:
	PressureFactory(Board board, const std::pair<short, short>& piecePosition);
	
	bool canAttack(const std::pair<short, short> destination);
	std::unordered_map<std::pair<short, short>, MoveType, PairHash> getMoves();

protected:
	void generateMoves();
	void generateVerticalMoves(bool multiMove = false);
	void generateHorizontalMoves(bool multiMove = false);
	void generateDiagonalMoves(bool multiMove = false);
	void generateJumpMoves();
	bool addMove(short x, short y, MoveType moveType = MoveType::Normal);
	bool addMove(const std::pair<short, short>& position, MoveType moveType = MoveType::Normal);
	bool isEnemy(short x, short y);
	bool isEnemy(const std::pair<short, short>& position);

private:
	Board _board;
	Square& _selectedSquare;
	std::unordered_map<std::pair<short, short>, MoveType, PairHash> _moves;
};

}