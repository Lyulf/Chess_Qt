#pragma once

#include "square.hpp"
#include <string>

namespace chess {

class MoveInfo {
public:
    MoveInfo(
        PieceColor turn,
		const std::string& algebraicNotation
    );
    PieceColor getTurn() const;
	std::string getAlgebraicNotation() const;


private:
    PieceColor _turn;
	std::string _notation;
};

}
