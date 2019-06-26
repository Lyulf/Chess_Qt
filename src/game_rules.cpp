#include "../include/chess/engine/game_rules.hpp"
#include "../include/chess/engine/pressure_factory.hpp"
#include <algorithm>
#include <iterator>

using namespace chess;

GameRules::GameRules(const Game& game)
	: _moves() {
	selectGame(game);
}

void GameRules::selectGame(const Game& game) {
	_game = &game;
	updatePosition();
}

void GameRules::updatePosition() {
    _board = _game->getBoard();
    deselectSquare();
}

bool GameRules::selectSquare(const std::pair<short, short>& position) {
    if(!Board::positionExists(position)) {
		return false;
    }
    _currentSquare = &_board[position];
    Piece piece = _currentSquare->piece;
    if(piece.type != PieceType::None && piece.color == _game->getTurn()) {
        updatePossibleMoves();
        return true;
    }
    deselectSquare();
    return false;
}

void GameRules::deselectSquare() {
    _currentSquare = nullptr;
    _moves.clear();
}

MoveType GameRules::getMoveType(const std::pair<short, short>& destination) {
	auto move = _moves.find(destination);
	if(move != _moves.end()) {
		return move->second;
	} else {
		return MoveType::None;
	}
}

bool GameRules::isCheck(PieceColor turn) {
	std::pair<short, short> kingPosition;
    if(turn == PieceColor::White) {
		kingPosition = _game->getWhiteKingSquare()->getPosition();
	} else {
		kingPosition = _game->getBlackKingSquare()->getPosition();
	}
	return enemyCanAttack(_game->getBoard(), kingPosition);
}

Square* GameRules::getSelectedSquare() {
    return _currentSquare;
}

std::unordered_set<std::pair<short, short>, PairHash> GameRules::getPossibleMoves() {
	std::unordered_set<std::pair<short, short>, PairHash> moves;
	std::transform(_moves.begin(), _moves.end(),
		std::inserter(moves, moves.end()), [](auto pair){ return pair.first; });
	return moves;
}

void GameRules::updatePossibleMoves() {
    _moves.clear();
	setPossibleMoves();
	removeInvalidMoves();
}

void GameRules::setPossibleMoves() {
	PressureFactory pressureFactory(_game->getBoard(), _currentSquare->getPosition());
	auto m = pressureFactory.getMoves();
	_moves.insert(m.begin(), m.end());
    Piece piece = _currentSquare->piece;
    if(piece.type == PieceType::Pawn) {
		short x = _currentSquare->getX();
		short y = _currentSquare->getY();
		short step = _currentSquare->piece.color == PieceColor::White ? 1 : -1;
		MoveType moveType = (y + step) == 0 || (y + step) == 7 ? MoveType::Promotion : MoveType::Normal;
		if(_board[x][y + step].piece.type == PieceType::None) {
            addMove(x, y + step, moveType);
			if(y == 1 || y == 6) {
				if(Board::positionExists(x, y + 2 * step)) {
                    if(_board[x][y + 2 * step].piece.type == PieceType::None) {
						addMove(x, y + 2 * step, MoveType::PawnDouble);
					}
				}
			}
        }

        if(!isEnemy(x + 1, y + step)) {
            removeMove(x + 1, y + step);
		}

        if(!isEnemy(x - 1, y + step)) {
            removeMove(x - 1, y + step);
        }

        Square* enPassant = _game->getEnPassantSquare();
        if(enPassant) {
            if(enPassant->getY() == y + step) {
                if(enPassant->getX() == x + 1) {
                    addMove(x + 1, y + step, MoveType::EnPassantCapture);
                } else if(enPassant->getX() == x - 1) {
                    addMove(x - 1, y + step, MoveType::EnPassantCapture);
                }
            }
        }
    } else if(piece.type == PieceType::King) {
        if(_currentSquare->getX() == 4) {
            if(piece.color == PieceColor::White) {
                if(_currentSquare->getY() == 0) {
                    if(whiteCanCastleA()) {
                        addMove(2, 0, MoveType::Castle);
                    }
                    if(whiteCanCastleH()) {
                        addMove(6, 0, MoveType::Castle);
                    }
                }
            } else {
                if(_currentSquare->getY() == 7) {
                    if(blackCanCastleA()) {
                        addMove(2, 7, MoveType::Castle);
                    }
                    if(blackCanCastleH()) {
                        addMove(6, 7, MoveType::Castle);
                    }
                }
            }
        }
    }
}

void GameRules::removeInvalidMoves() {
	auto moveCopy = _moves;
	if(_currentSquare->piece.type == PieceType::King) {
		for(auto movePair : moveCopy) {
			Game game = *_game;
			game.move(_currentSquare->getPosition(), movePair.first, movePair.second);
			if(enemyCanAttack(game.getBoard(), movePair.first)) {
				removeMove(movePair.first);
			}
		}
	} else {
		std::pair<short, short> kingPosition;
		if(_currentSquare->piece.color == PieceColor::White) {
			kingPosition = _game->getWhiteKingSquare()->getPosition();
		} else {
			kingPosition = _game->getBlackKingSquare()->getPosition();
		}
		std::pair<short, short> piecePosition = _currentSquare->getPosition();

		for(auto movePair : moveCopy) {
			Game game = *_game;
			game.move(piecePosition, movePair.first, movePair.second);
			if(enemyCanAttack(game.getBoard(), kingPosition)) {
				removeMove(movePair.first);
			}
		}
	}

}

bool GameRules::whiteCanCastleA() {
	if(_game->canWhiteCastleA()) {
        if(_board[0][0].piece.type != PieceType::Rook) {
            return false;
        }
		for(short x = 1; x <= _game->getWhiteKingSquare()->getX(); x++) {
			Piece piece = _board[x][0].piece;
			if(piece.type != PieceType::None) {
				if(piece.type != PieceType::King || piece.color != PieceColor::White) {
					return false;
				}
			}
			if(enemyCanAttack(_game->getBoard(), std::make_pair(x, 0))) {
				return false;
			}
        }
		return true;
	}
	return false;
}
bool GameRules::whiteCanCastleH() {
    if(_board[7][0].piece.type != PieceType::Rook) {
        return false;
    }
	if(_game->canWhiteCastleH()) {
		for(short x = 6; x >= _game->getWhiteKingSquare()->getX(); x--) {
			Piece piece = _board[x][0].piece;
			if(piece.type != PieceType::None) {
				if(piece.type != PieceType::King || piece.color != PieceColor::White) {
					return false;
				}
			}
			if(enemyCanAttack(_game->getBoard(), std::make_pair(x, 0))) {
				return false;
			}
		}
		return true;
	}
	return false;
}

bool GameRules::blackCanCastleA() {
    if(_board[0][7].piece.type != PieceType::Rook) {
        return false;
    }
	if(_game->canBlackCastleA()) {
		for(short x = 1; x <= _game->getBlackKingSquare()->getX(); x++) {
			Piece piece = _board[x][7].piece;
			if(piece.type != PieceType::None) {
				if(piece.type != PieceType::King || piece.color != PieceColor::Black) {
					return false;
				}
			}
			if(enemyCanAttack(_game->getBoard(), std::make_pair(x, 7))) {
				return false;
			}
		}
		return true;
	}
	return false;
}
bool GameRules::blackCanCastleH() {
    if(_board[7][7].piece.type != PieceType::Rook) {
        return false;
    }
	if(_game->canBlackCastleH()) {
		for(short x = 6; x >= _game->getBlackKingSquare()->getX(); x--) {
			Piece piece = _board[x][7].piece;
			if(piece.type != PieceType::None) {
				if(piece.type != PieceType::King || piece.color != PieceColor::Black) {
					return false;
				}
			}
			if(enemyCanAttack(_game->getBoard(), std::make_pair(x, 7))) {
				return false;
			}
		}
		return true;
	}
	return false;
}

Square* GameRules::getPinningSquare(short originX, short originY, short stepX, short stepY) {
	short outOfRangeX = originX;
	short outOfRangeY = originY;
	if(stepX > 0) {
		stepX = 1;
		outOfRangeX = 7;
	} else if( stepX < 0) {
		stepX = -1;
		outOfRangeX = 0;
	}
	if(stepY > 0) {
		stepY = 1;
		outOfRangeY = 7;
	} else if( stepY < 0) {
		stepY = -1;
		outOfRangeY = 0;
	}

	for(short x = originX; x != outOfRangeX; x += stepX) {
		for(short y = originY; y != outOfRangeY; y += stepY) {
			Square* square = &_board[x][y];
			if(square->piece.type != PieceType::None) {
				return square;
			}
		}
	}
	return nullptr;
}

bool GameRules::enemyCanAttack(Board board, const std::pair<short, short>& destination) {
	for(short x = 0; x < 8; x++) {
		for(short y = 0; y < 8; y++) {
			Piece piece = board[x][y].piece;
			if(piece.type != PieceType::None) {
				if(piece.color != _game->getTurn()) {
					PressureFactory enemy(board, std::make_pair(x, y));
					if(enemy.canAttack(destination)) {
						return true;
					}
				}
			}
		}
	}
	return false;
}

void GameRules::addMove(const std::pair<short, short>& destination, MoveType moveType) {
	if(!Board::positionExists(destination)) {
		return;
	}

	Piece targetPiece = _board[destination].piece;
	if(targetPiece.type != PieceType::None) {
		if(targetPiece.color == _currentSquare->piece.color) {
			return;
		}
	}
	_moves.insert(std::make_pair(destination, moveType));
}

void GameRules::removeMove(const std::pair<short, short>& destination) {
	_moves.erase(destination);	
}

bool GameRules::isEnemy(const std::pair<short, short>& position) {
	if(!Board::positionExists(position)) {
		return false;
	}
	Piece piece = _board[position].piece;
	if(piece.type != PieceType::None) {
		return _currentSquare->piece.color != piece.color;
	} else {
		return false;
	}
}

void GameRules::addMove(short x, short y, MoveType moveType) {
	addMove(std::make_pair(x, y), moveType);
}

void GameRules::removeMove(short x, short y) {
	_moves.erase(std::make_pair(x, y));	
}

bool GameRules::isEnemy(short x, short y) {
	return isEnemy(std::make_pair(x, y));
}
