#pragma once

#include "game_rules.hpp"
#include "move_info.hpp"

namespace chess {

class Engine {
public:
    Engine();

	bool selectPiece(const std::pair<short, short>& piecePosition);
    Square* getSelectedSquare();
    void deselectPiece();
    bool move(const std::pair<short, short>& destination);

    bool selectPosition(size_t position);
    size_t previousPosition();
    size_t nextPosition();

	void setBoard(const std::string& fen);
    Board getBoard();
	GameState getGameState();

    short getStartingMoveIndex();
    std::pair<short, short> getCheckPosition();
	std::unordered_set<std::pair<short, short>, PairHash> getPossibleMoves();
    std::vector<MoveInfo> getMoveHistory();
    std::vector<Piece> getMaterialImbalance();

protected:
    void cutFutureMoves();
    void setGameState();
    MoveInfo createMoveInfo(const std::pair<short, short>& destination, MoveType moveType, PieceColor turn);

private:
    std::vector<Game> _positionHistory;
    std::vector<MoveInfo> _moveHistory;
    size_t _currentGameIndex;
	Game _currentGame;
    GameRules _gameRules;
};

}
