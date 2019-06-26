#include "../include/chess/engine/engine.hpp"
#include <algorithm>

using namespace chess;

Engine::Engine()
    : _positionHistory(), _moveHistory(), _currentGameIndex(0),
      _currentGame(), _gameRules(_currentGame) {
    _positionHistory.push_back(_currentGame);
    setGameState();
}

bool Engine::selectPiece(const std::pair<short, short>& piecePosition) {
    if(_currentGame.getGameState() == GameState::Playing) {
        return _gameRules.selectSquare(piecePosition);
    } else {
        return false;
    }
}

Square* Engine::getSelectedSquare() {
    return _gameRules.getSelectedSquare();
}

void Engine::deselectPiece() {
    _gameRules.deselectSquare();
}

bool Engine::move(const std::pair<short, short>& destination) {
    if(_currentGame.getGameState() != GameState::Playing) {
        return false;
    }
    auto moveType = _gameRules.getMoveType(destination);
    if(moveType != MoveType::None) {
        cutFutureMoves();
        MoveInfo info = createMoveInfo(destination, moveType, _currentGame.getTurn());
        _moveHistory.push_back(info);
        auto origin = _gameRules.getSelectedSquare()->getPosition();
        _currentGame.move(origin, destination, moveType);
        setGameState();
        _gameRules.updatePosition();
        _positionHistory.push_back(_currentGame);
        _currentGameIndex++;
		return true;
	}
	return false;
}

void Engine::cutFutureMoves() {
    auto replacedPosition = _positionHistory.begin();
    auto replacedMove = _moveHistory.begin();
    std::advance(replacedPosition, _currentGameIndex + 1);
    std::advance(replacedMove, _currentGameIndex);
    _positionHistory.erase(replacedPosition, _positionHistory.end());
    _moveHistory.erase(replacedMove, _moveHistory.end());
}

bool Engine::selectPosition(size_t position) {
    if(position < _positionHistory.size()) {
        _currentGameIndex = position;
        _currentGame = _positionHistory[_currentGameIndex];
        _gameRules.updatePosition();
        return true;
    } else {
        return false;
    }
}

size_t Engine::previousPosition() {
    if(_currentGameIndex != 0) {
        _currentGame = _positionHistory[--_currentGameIndex];
        _gameRules.updatePosition();
        return _currentGameIndex;
    }
    return -1;
}

size_t Engine::nextPosition() {
    if(_currentGameIndex < _positionHistory.size() - 1) {
        _currentGame = _positionHistory[++_currentGameIndex];
        _gameRules.updatePosition();
        return _currentGameIndex;
    }
    return -1;
}

void Engine::setBoard(const std::string& fen) {
    Game game(fen); // if fen is invalid it will throw invalid_argument exception
    _currentGame = game;
    setGameState();
    _currentGameIndex = 0;
    _positionHistory.clear();
    _positionHistory.push_back(_currentGame);
    _moveHistory.clear();
    _gameRules.updatePosition();
}

Board Engine::getBoard() {
	return _currentGame.getBoard();
}

GameState Engine::getGameState() {
    return _currentGame.getGameState();
}

void Engine::setGameState() {
    auto board = _currentGame.getBoard();
    short noRepetitions = 1;
    for(short i = 0; i <= _currentGameIndex; i++) {
        if(_positionHistory[i].getBoard() == board) {
            noRepetitions++;
        }
    }
    if(noRepetitions >= 3) {
        _currentGame.setGameState(GameState::Draw);
        return;
    }
    GameRules gameRules(_currentGame);
    std::vector<Square> pieces;
    bool canMove = false;
	for(short x = 0; x < 8; x++) {
		for(short y = 0; y < 8; y++) {
            auto square = board[x][y];
            if(square.piece.type != PieceType::None && square.piece.type != PieceType::King) {
                pieces.push_back(square);
            }
            if(gameRules.selectSquare(std::make_pair(x, y))) {
                if(!gameRules.getPossibleMoves().empty()) {
                    canMove = true;
				}
			}
		}
    }

    GameState state;
    if(canMove) {
        if(_currentGame.getNoHalfMoves() >= 100) {
            state =  GameState::Draw;
        } else {
            if(pieces.empty()) {
                state = GameState::Draw;
            } else if(pieces.size() == 1) {
                PieceType type = pieces.front().piece.type;
                if(type == PieceType::Bishop || type == PieceType::Knight) {
                    state = GameState::Draw;
                } else {
                    state = GameState::Playing;
                }
            } else if(pieces.size() == 2) {
                if(pieces[0].piece.type == PieceType::Bishop && pieces[1].piece.type == PieceType::Bishop) {
                    if((pieces[0].getX() + pieces[0].getY()) % 2 == (pieces[1].getX() + pieces[1].getY()) % 2) {
                        state = GameState::Draw;
                    } else {
                        state = GameState::Playing;
                    }
                } else {
                    state = GameState::Playing;
                }
            } else {
                state = GameState::Playing;
            }
        }
    } else {
        if(_gameRules.isCheck(_currentGame.getTurn())) {
            if(_currentGame.getTurn() == PieceColor::White) {
                state = GameState::BlackWin;
            } else {
                state = GameState::WhiteWin;
            }
        } else {
            state = GameState::Draw;
        }
    }
    _currentGame.setGameState(state);
}

short Engine::getStartingMoveIndex() {
    return _positionHistory.front().getNoMoves();
}

std::pair<short, short> Engine::getCheckPosition() {
    auto checkedKingColor = _currentGame.getTurn();
    if(_gameRules.isCheck(checkedKingColor)) {
        if(checkedKingColor == PieceColor::White) {
            return _currentGame.getWhiteKingSquare()->getPosition();
        } else {
            return _currentGame.getBlackKingSquare()->getPosition();
        }
    }
    return std::make_pair(-1, -1);
}

std::unordered_set<std::pair<short, short>, PairHash> Engine::getPossibleMoves() {
	return _gameRules.getPossibleMoves();
}

std::vector<MoveInfo> Engine::getMoveHistory() {
    return _moveHistory;
}

std::vector<Piece> Engine::getMaterialImbalance() {
    std::vector<Piece> whitePieces;
    std::vector<Piece> blackPieces;
    auto board = _currentGame.getBoard();
    for(short x = 0; x < 8; x++) {
        for(short y = 0; y < 8; y++) {
            Piece piece = board[x][y].piece;
            if(piece.type != PieceType::None) {
                if(piece.color == PieceColor::White) {
                    whitePieces.push_back(piece);
                } else {
                    blackPieces.push_back(piece);
                }
            }
        }
    }
    std::sort(
                whitePieces.begin(), whitePieces.end(),
                [](auto a, auto b){return static_cast<int>(a.type) < static_cast<int>(b.type);}
    );
    std::sort(
                blackPieces.begin(), blackPieces.end(),
                [](auto a, auto b){return static_cast<int>(a.type) < static_cast<int>(b.type);}
    );
    std::vector<Piece> imbalance;
    std::set_difference(
                whitePieces.begin(), whitePieces.end(),
                blackPieces.begin(), blackPieces.end(),
                std::back_inserter(imbalance),
                [](auto a, auto b){return static_cast<int>(a.type) < static_cast<int>(b.type);}
    );
    std::set_difference(
                blackPieces.begin(), blackPieces.end(),
                whitePieces.begin(), whitePieces.end(),
                std::back_inserter(imbalance),
                [](auto a, auto b){return static_cast<int>(a.type) < static_cast<int>(b.type);}
    );
    return imbalance;
}

MoveInfo Engine::createMoveInfo(const std::pair<short, short>& destination, MoveType moveType, PieceColor turn) {
    if(moveType == MoveType::Castle) {
        if(destination.first == 2) {
            return MoveInfo(turn, "O-O-O");
        } else {
            return MoveInfo(turn, "O-O");
        }
    }
    auto selectedSquare = *_gameRules.getSelectedSquare();
    auto board = _currentGame.getBoard();
    Game game(_currentGame);
    GameRules gameRules(game);
    bool addFile = false;
    bool addRank = false;
    for(short x = 0; x < 8; x++) {
        for(short y = 0; y < 8; y++) {
            auto pair = std::make_pair(x, y);
            if(pair == selectedSquare.getPosition()) {
                continue;
            }
            if(board[x][y].piece.type == selectedSquare.piece.type) {
                if(gameRules.selectSquare(std::make_pair(x, y))) {
                    auto moves = gameRules.getPossibleMoves();
                    if(moves.find(destination) != moves.end()) {
                        if(x == selectedSquare.getX()) {
                            addFile = true;
                        }
                        if(y == selectedSquare.getY()) {
                            addRank = true;
                        }
                    }
                }
            }
        }
    }
    std::string algebraicNotation;
    switch (selectedSquare.piece.type) {
    case PieceType::King:
        algebraicNotation += 'K';
        break;
    case PieceType::Queen:
        algebraicNotation += 'Q';
        break;
    case PieceType::Rook:
        algebraicNotation += 'R';
        break;
    case PieceType::Bishop:
        algebraicNotation += 'B';
        break;
    case PieceType::Knight:
        algebraicNotation += 'N';
        break;
    default:
        break;
    }
    if(addFile) {
        algebraicNotation += Square::getFile(selectedSquare.getPosition());
    }
    if(addRank) {
        algebraicNotation += Square::getRank(selectedSquare.getPosition());
    }
    if(board[destination].piece.type != PieceType::None || moveType == MoveType::EnPassantCapture) {
        if(selectedSquare.piece.type == PieceType::Pawn) {
            algebraicNotation += Square::getFile(selectedSquare.getPosition());
        }
        algebraicNotation += "x";
    }
    algebraicNotation += Square::convertPosition(destination);
    if(moveType == MoveType::Promotion) {
        algebraicNotation += "=Q";
    }
    game.move(selectedSquare.getPosition(), destination, moveType);
    gameRules.updatePosition();
    if(gameRules.isCheck((turn == PieceColor::White ? PieceColor::Black : PieceColor::White))) {
        bool canMove = false;
        for(short x = 0; x < 8; x++) {
            for(short y = 0; y < 8; y++) {
                if(gameRules.selectSquare(std::make_pair(x, y))) {
                    if(!gameRules.getPossibleMoves().empty()) {
                        canMove = true;
                    }
                }
            }
        }
        if(canMove) {
            algebraicNotation += "+";
        } else {
            algebraicNotation += "#";
        }

    }
    return MoveInfo(turn, algebraicNotation);
}
