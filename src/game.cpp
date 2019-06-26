#include "../include/chess/engine/game.hpp"
#include "../include/chess/engine/string_tok.hpp"
#include <utility>

using namespace chess;

Game::Game(const std::string& fen)
    : _board(),
	  _whiteCastleA(false),
	  _whiteCastleH(false),
	  _blackCastleA(false),
      _blackCastleH(false),
      _state(GameState::Playing),
      _whiteKingSquare(nullptr),
      _blackKingSquare(nullptr) {
	StringTok st(fen);
	st.addDelimiter("/");
	st.addDelimiter(" ");
	std::string s;
	try {
	for(short y = 7; y >= 0; y--) {
		short x = 0;
		s = st.getNext();
		for(char c : s) {
			if(isdigit(c)) {
				if(c == '0' || c == '9') {
					throw std::invalid_argument("");
				}
				x += c - '0';
				continue;
			}
			Piece piece(c);
			if(piece.type == PieceType::King) {
				if(piece.color == PieceColor::White) {
					if(!_whiteKingSquare) {
						_whiteKingSquare = &_board[x][y];
					} else {
						throw std::invalid_argument("");
					}
				} else {
					if(!_blackKingSquare) {
						_blackKingSquare = &_board[x][y];
					} else {
						throw std::invalid_argument("");
					}
				}
			}
			_board[x++][y].piece = piece;
		}
	}
		s = st.getNext();
		if(s == "w") {
			_turn = PieceColor::White;
		} else if(s == "b") {
			_turn = PieceColor::Black;
		} else {
			throw std::invalid_argument("");
		}
		s = st.getNext();

		for(char c : s) {
			switch(c) {
			case 'K':
				_whiteCastleH = true;
				break;
			case 'Q':
				_whiteCastleA = true;
				break;
			case 'k':
				_blackCastleH = true;
				break;
			case 'q':
				_blackCastleA = true;
				break;
			case '-':
				break;
			default:
				throw std::invalid_argument("");
			}
		}

		if(_whiteCastleA || _whiteCastleH) {
			if(_whiteKingSquare->getX() != 4) {
				throw std::invalid_argument("");
			}
			if(_whiteKingSquare->getY() != 0) {
				throw std::invalid_argument("");
			}
		}

		if(_blackCastleA || _blackCastleH) {
			if(_blackKingSquare->getX() != 4) {
				throw std::invalid_argument("");
			}
			if(_blackKingSquare->getY() != 7) {
				throw std::invalid_argument("");
			}
		}

		s = st.getNext();
		if(s.length() == 2) {
			_enPassantSquare = &_board[Square::convertPosition(s)];
		} else if(s[0] == '-') {
			_enPassantSquare = nullptr;
		} else {
			throw std::invalid_argument("");
		}
		s = st.getNext();
        if(!s.empty()) {
            _noHalfMoves = static_cast<short>(stoi(s));
        } else {
            _noHalfMoves = 0;
        }
		s = st.getNext();
        if(!s.empty()) {
            _noMoves = static_cast<short>(stoi(s));
        } else {
            _noMoves = 1;
        }
    } catch(std::invalid_argument&) {
		throw std::invalid_argument("Error: Invalid character in fen");
	}
}

Game::Game(const Game& other)
    : _board(other._board),
	  _turn(other._turn),
	  _noHalfMoves(other._noHalfMoves),
	  _noMoves(other._noMoves),
	  _whiteCastleA(other._whiteCastleA),
	  _whiteCastleH(other._whiteCastleH),
	  _blackCastleA(other._blackCastleA),
      _blackCastleH(other._blackCastleH),
      _state(other._state),
	  _whiteKingSquare(nullptr),
      _blackKingSquare(nullptr),
      _enPassantSquare(nullptr) {
	if(other._enPassantSquare) {
		_enPassantSquare = &_board[other._enPassantSquare->getPosition()];
	}
	if(other._whiteKingSquare) {
		_whiteKingSquare = &_board[other._whiteKingSquare->getPosition()];
	}
	if(other._blackKingSquare) {
		_blackKingSquare = &_board[other._blackKingSquare->getPosition()];
	}
}

Game& Game::operator=(const Game& other) {
	Game temp(other);
	temp.swap(*this);
	if(other._enPassantSquare) {
		_enPassantSquare = &_board[other._enPassantSquare->getPosition()];
    } else {
        _enPassantSquare = nullptr;
    }

	if(other._whiteKingSquare) {
		_whiteKingSquare = &_board[other._whiteKingSquare->getPosition()];
    } else {
        _enPassantSquare = nullptr;
    }

	if(other._blackKingSquare) {
		_blackKingSquare = &_board[other._blackKingSquare->getPosition()];
    } else {
        _blackKingSquare = nullptr;
    }

	return *this;
}

void Game::swap(Game& other) {
    std::swap(_board, other._board);
	std::swap(_turn, other._turn);
	std::swap(_noHalfMoves, other._noHalfMoves);
	std::swap(_noMoves, other._noMoves);
	std::swap(_whiteCastleA, other._whiteCastleA);
	std::swap(_whiteCastleH, other._whiteCastleH);
	std::swap(_blackCastleA, other._blackCastleA);
	std::swap(_blackCastleH, other._blackCastleH);
    std::swap(_state, other._state);
}

void Game::move(
	const std::pair<short, short>& origin,
	const std::pair<short, short>& destination,
	const MoveType& moveType
) {
	if(moveType == MoveType::None) {
		return;
    }
	movePiece(origin, destination);
    switch(moveType) {
    case MoveType::PawnDouble:
        _enPassantSquare = &_board[origin.first][(destination.second + origin.second) / 2];
		break;
	case MoveType::Promotion:
        _board[destination].piece.type = PieceType::Queen;
		break;
	case MoveType::EnPassantCapture:
	{
		std::pair<short, short> capturedPiece = _enPassantSquare->getPosition();
		capturedPiece.second -= _turn == PieceColor::White ? 1 : -1;
        _board[capturedPiece].piece.type = PieceType::None;
		break;
	}
	case MoveType::Castle:
		if(destination.first == 2) {
			auto rookOrigin = std::make_pair(0, destination.second);
			auto rookDestination = destination;
			rookDestination.first++;
			movePiece(rookOrigin, rookDestination);
		} else {
			auto rookOrigin = std::make_pair(7, destination.second);
			auto rookDestination = destination;
			rookDestination.first--;
			movePiece(rookOrigin, rookDestination);
		}
		break;
	default:
		break;
	}

	if(moveType != MoveType::PawnDouble) {
		_enPassantSquare = nullptr;
    }
	_turn = _turn == PieceColor::White ? PieceColor::Black : PieceColor::White;
}

void Game::movePiece(
	const std::pair<short, short>& origin,
	const std::pair<short, short>& destination
) {
    Piece movedPiece = _board[origin].piece;
    Piece capturedPiece = _board[destination].piece;
    _board[destination].piece = movedPiece;
	_board[origin].piece.type = PieceType::None;
    if(movedPiece.type == PieceType::King) {
        if(movedPiece.color == PieceColor::White) {
			_whiteKingSquare = &_board[destination];
			_whiteCastleA = false;
			_whiteCastleH = false;
		} else {
			_blackKingSquare = &_board[destination];
			_blackCastleA = false;
			_blackCastleH = false;
		}
	} else if(origin.second == 0) {
		if(origin.first == 0) {
			_whiteCastleA = false;
		} else if(origin.first == 7) {
			_whiteCastleH = false;
		}
	} else if(origin.second == 7) {
		if(origin.first == 0) {
			_blackCastleA = false;
		} else if(origin.first == 7) {
			_blackCastleH = false;
		}
	}

    _noMoves++;
    if(movedPiece.type == PieceType::Pawn || capturedPiece.type != PieceType::None) {
        _noHalfMoves = 0;
    } else {
        _noHalfMoves++;
    }
}


void Game::setGameState(GameState state) {
    _state = state;
}

GameState Game::getGameState() {
    return _state;
}

Board Game::getBoard() const {
	return _board;
}

PieceColor Game::getTurn() const {
	return _turn;
}

Square* Game::getWhiteKingSquare() const {
	return _whiteKingSquare;
}
Square* Game::getBlackKingSquare() const {
	return _blackKingSquare;
}

Square* Game::getEnPassantSquare() const {
	return _enPassantSquare;
}

short Game::getNoHalfMoves() const {
	return _noHalfMoves;
}

short Game::getNoMoves() const {
	return _noMoves;
}

bool Game::canWhiteCastleA() const {
	return _whiteCastleA;
}

bool Game::canWhiteCastleH() const {
	return _whiteCastleH;
}

bool Game::canBlackCastleA() const {
	return _blackCastleA;
}

bool Game::canBlackCastleH() const {
	return _blackCastleH;
}
