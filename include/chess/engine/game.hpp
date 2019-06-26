#pragma once

#include <stack>
#include <memory>
#include <string>
#include <vector>
#include "board.hpp"
#include "move_type.hpp"
#include "game_state.hpp"

namespace chess {

class Game {
public:
    friend class GameRules;

    Game(const std::string& fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq -");
    Game(const Game& other);

    virtual ~Game() = default;

    Game& operator=(const Game& other);

    virtual void move(
            const std::pair<short, short>& origin,
            const std::pair<short, short>& destination,
            const MoveType& moveType
    );

    void setGameState(GameState state);
    GameState getGameState();

    Board getBoard() const;
    PieceColor getTurn() const;
    Square* getWhiteKingSquare() const;
    Square* getBlackKingSquare() const;
    Square* getEnPassantSquare() const;
    short getNoHalfMoves() const;
    short getNoMoves() const;

    bool canWhiteCastleA() const;
    bool canWhiteCastleH() const;
    bool canBlackCastleA() const;
    bool canBlackCastleH() const;
	
protected:
    void movePiece(
            const std::pair<short, short>& origin,
            const std::pair<short, short>& destination
    );
private:
    void swap(Game& other);

    Board _board;
    PieceColor _turn;
    short _noHalfMoves;
    short _noMoves;
    bool _whiteCastleA;
    bool _whiteCastleH;
    bool _blackCastleA;
    bool _blackCastleH;
    GameState _state;
    Square* _whiteKingSquare;
    Square* _blackKingSquare;
    Square* _enPassantSquare;
};

}
