#pragma once

#include "../include/chess/engine/square.hpp"
#include <string>
#include <sstream>

using namespace chess;

Square::Square(const short& x, const short& y)
	: _x(x), _y(y), piece() { }

Square::Square(const std::pair<short, short>& position)
	: Square(position.first, position.second) { }

Square::Square(const std::string& position) 
	: Square(convertPosition(position)) {}

Square::operator std::string() {
	return convertPosition(getPosition());
}

short Square::getX() {
	return _x;
}

short Square::getY() {
	return _y;
}

std::pair<short, short> Square::getPosition() {
	return std::make_pair(_x, _y);
}

std::pair<short, short> Square::convertPosition(const std::string& position) {
	if(position.length() != 2) {
		throw std::invalid_argument("Error: Position has invalid length.");
	}
	if(position[0] < 'a' || position[0] > 'h') {
		throw std::invalid_argument("Error: First part of position is invalid.");
	}
	if(position[1] < '1' || position[1] > '8') {
		throw std::invalid_argument("Error: Second part of position is invalid.");
	}
	return std::make_pair(position[0] - 'a', position[1] - '1');
}

std::string Square::convertPosition(const std::pair<short, short>& position) {
    char convertedPosition[3] {getFile(position), getRank(position)};
	return std::string(convertedPosition);
}

char Square::getFile(const std::pair<short, short>& position) {
    if(position.first < 0 || position.first > 7) {
        throw std::invalid_argument("Error: First part of position is invalid.");
    }
    return 'a' + static_cast<char>(position.first);
}

char Square::getRank(const std::pair<short, short>& position) {
    if(position.second < 0 || position.second > 7) {
        throw std::invalid_argument("Error: Second part of position is invalid.");
    }
    return '1' + static_cast<char>(position.second);
}

std::ostream& chess::operator<<(std::ostream& os, Square square) {
	return os << static_cast<std::string>(square);
}
