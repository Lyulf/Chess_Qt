#include "../include/chess/victory_screen.hpp"

#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>

using namespace chess;

VictoryScreen::VictoryScreen(GameState state, QWidget* parent)
    : QDialog(parent) {

    setWindowTitle("Result");
    setFixedSize(330, 150);
    setStyleSheet("VictoryScreen {background:qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgb(222, 222, 222), stop:1 rgb(200, 200, 200));}");

    QVBoxLayout* mainVLayout = new QVBoxLayout(this);
    QHBoxLayout* buttonHLayout = new QHBoxLayout();
    QLabel* gameResult = new QLabel(this);
    QPushButton* resetButton = new QPushButton("Reset", this);
    QPushButton* backButton = new QPushButton("Back", this);
    mainVLayout->addWidget(gameResult);
    mainVLayout->addItem(buttonHLayout);
    buttonHLayout->addStretch(1);
    buttonHLayout->addWidget(resetButton, 2);
    buttonHLayout->addStretch(1);
    buttonHLayout->addWidget(backButton, 2);
    buttonHLayout->addStretch(1);

    connect(resetButton, &QPushButton::clicked, this, &VictoryScreen::accept);
    connect(backButton, &QPushButton::clicked, this, &VictoryScreen::reject);

    gameResult->setStyleSheet("QLabel { color: #000000; font: 40pt; font: bold; } ");

    QString winMsg;
    switch(state) {
    case GameState::WhiteWin:
        winMsg = "White won!";
        break;
    case GameState::BlackWin:
        winMsg = "Black won!";
        break;
    case GameState::Draw:
        winMsg = "Draw!";
        break;
    default:
        break;
    }
    gameResult->setText(QString("<p align=center>%1</p>").arg(winMsg));
}
