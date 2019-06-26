#ifndef VICTORY_SCREEN_HPP
#define VICTORY_SCREEN_HPP

#include <QDialog>
#include <QLabel>
#include "engine/game_state.hpp"

namespace chess {
    enum class GameState;
}

class VictoryScreen : public QDialog {
    Q_OBJECT
public:
    VictoryScreen(chess::GameState state, QWidget* parent = nullptr);
};

#endif // WIN_SCREEN_HPP
