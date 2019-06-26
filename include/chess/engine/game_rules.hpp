#pragma once

#include "game.hpp"
#include "pressure_factory.hpp"
#include <unordered_set>

namespace chess {

class GameRules {
public:
	GameRules(const Game& game);

	void selectGame(const Game& game);
	void updatePosition();

	bool selectSquare(const std::pair<short, short>& position);
        void deselectSquare();
	MoveType getMoveType(const std::pair<short, short>& destination);
    bool isCheck(PieceColor turn);

        Square* getSelectedSquare();
	std::unordered_set<std::pair<short, short>, PairHash> getPossibleMoves();
protected:
	void updatePossibleMoves();
        void setPossibleMoves();
	void removeInvalidMoves();

	bool whiteCanCastleA();
        bool whiteCanCastleH();
	bool blackCanCastleA();
	bool blackCanCastleH();

	Square* getPinningSquare(short originX, short originY, short stepX, short stepY);
	bool enemyCanAttack(Board board, const std::pair<short, short>& destination);
	void addMove(const std::pair<short, short>& destination, MoveType moveType);
	void removeMove(const std::pair<short, short>& destination);
	bool isEnemy(const std::pair<short, short>& position);

	void addMove(short x, short y, MoveType moveType = MoveType::Normal);
	void removeMove(short x, short y);
	bool isEnemy(short x, short y);
private:
	const Game* _game;
	Board _board;
	Square* _currentSquare;
	std::unordered_map<std::pair<short, short>, MoveType, PairHash> _moves;
};

}
