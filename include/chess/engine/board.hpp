#pragma once

#include <unordered_map>
#include "square.hpp"

namespace chess {

class BoardHelper;

class Board {
public:
	Board();
	Board(const Board& other) = default;

	Board& operator=(const Board& other) = default;

	BoardHelper operator[](short x);
	Square& operator[](const std::string& position);
    Square& operator[](const std::pair<short, short>& position);

    bool operator==(Board& other);
    bool operator!=(Board& other);

	static bool positionExists(short x, short y);
	static bool positionExists(const std::pair<short, short>& position);
private:
	std::unordered_map<std::pair<short, short>, Square, PairHash> _board;
};

class BoardHelper {
public:
	BoardHelper(Board& board, short x);
	Square& operator[](short y);
private:
	Board& _board;
	const short _x;
};

}
