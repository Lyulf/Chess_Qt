#pragma once

#include <string>
#include <memory>
#include <utility>
#include "piece.hpp"

namespace chess {

struct PairHash {
	inline std::size_t operator()(const std::pair<int,int> & pair) const {
		return pair.first*8+pair.second;
	}
};

struct Square {
	Square(const short& x, const short& y);
	Square(const std::pair<short, short>& position);
	Square(const std::string& position);
	Square(const Square& other) = default;

	short getX();
	short getY();
	std::pair<short, short> getPosition();

	operator std::string();

	static std::pair<short, short> convertPosition(const std::string& position);
	static std::string convertPosition(const std::pair<short, short>& position);
    static char getFile(const std::pair<short, short>& position);
    static char getRank(const std::pair<short, short>& position);


	Piece piece;
private:
	short _x, _y;
};

std::ostream& operator<<(std::ostream& os, Square square);

}
