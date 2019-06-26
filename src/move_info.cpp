#include "../include/chess/engine/move_info.hpp"
#include <sstream>

using namespace chess;

MoveInfo::MoveInfo(
        PieceColor turn,
        const std::string& algebraicNotation
) : _turn(turn), _notation(algebraicNotation) {}


PieceColor MoveInfo::getTurn() const {
    return _turn;
}

std::string MoveInfo::getAlgebraicNotation() const {
	return _notation;
}
