#include "../include/chess/engine/board.hpp"

using namespace chess;

BoardHelper::BoardHelper(Board& board, short x) 
	: _board(board), _x(x) {}

Square& BoardHelper::operator[](short y) {
	return _board[std::make_pair(_x, y)];
}

Board::Board() {
	for(short y = 0; y < 8; y++) {
		for(short x = 0; x < 8; x++) {
			_board.insert(std::make_pair(std::make_pair(x, y), Square(x, y)));
		}
	}
}

BoardHelper Board::operator[](short x) {
	return BoardHelper(*this, x);
}

Square& Board::operator[](const std::string& position) {
	return operator[](Square::convertPosition(position));
}

Square& Board::operator[](const std::pair<short, short>& position) {
	return _board.find(position)->second;
}

bool Board::operator==(Board& other) {
    for(short x = 0; x < 8; x++) {
        for(short y = 0; y < 8; y++) {
            auto position = std::make_pair(x, y);
            if(operator[](position).piece != other[position].piece) {
                return false;
            }
        }
    }
    return true;
}

bool Board::operator!=(Board& other) {
    return !(*this == other);
}

bool Board::positionExists(short x, short y) {
	if(x > 7 || x < 0) {
		return false;
	} else if(y > 7 || y < 0) {
		return false;
	} else {
		return true;
	}
}

bool Board::positionExists(const std::pair<short, short>& position) {
	return positionExists(position.first, position.second);
}
