#ifndef CHESS_HPP
#define CHESS_HPP

#include <QMainWindow>
#include <QByteArray>
#include "include/chess/engine/engine.hpp"

namespace Ui {
class Chess;
}

class Chess : public QMainWindow {
    Q_OBJECT

public:
    explicit Chess(QWidget *parent = nullptr);
    ~Chess();

protected:
    void resizeEvent(QResizeEvent* event);
    void showEvent(QShowEvent* event);
    void changeEvent(QEvent* event);
    void resizeBoard();
    void createBoard();
    void setStyle(short x, short y, QString squareArgs, QString additionalStyle = "");
    void setBackground(short x, short y);
    void setBorder(short x, short y, QString borderColor, QString additionalStyle = "");
    void setMovePosition(short x, short y);
    void setCheck();
    void updateBackgroundColor();
    void loadPieceSvgs();
    void updateBoard();
    void clearSelection();
    void updateMoveHistory();
    void createImbalanceGrid();
    void updateImbalanceGrid();
    void clearLayout(QLayout* layout, bool deleteWidgets = true);
    void generateBoard(const std::string& fen);
    void showWin(chess::GameState state);

protected slots:
    void clickSquare(short x, short y);
    bool selectPosition(int move, int color);
    void previousPosition();
    void nextPosition();

private slots:
    void on_actionBoardGeneration_triggered();
    void on_actionAboutCreator_triggered();
    void on_actionCommands_triggered();
    void on_actionResetBoard_triggered();

private:
    short convertY(short y);

    Ui::Chess* _ui;
    chess::Engine _engine;
    std::vector<QByteArray> _pieceSvgContents[2];
    QString _lightColor, _darkColor;
    QColor _borderBackground;
    bool _whiteIsFront;
};

#endif // CHESS_H
