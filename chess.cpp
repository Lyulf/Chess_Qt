#include "chess.hpp"
#include "ui_chess.h"
#include "include/chess/square_widget.hpp"
#include <QFile>
#include <QResizeEvent>
#include <algorithm>
#include <QShortcut>
#include <QTimer>
#include <QMessageBox>
#include <QInputDialog>
#include <include/chess/victory_screen.hpp>


using namespace chess;

Chess::Chess(QWidget* parent) :
    QMainWindow(parent),
    _ui(new Ui::Chess),
    _engine(),
    _lightColor("rgb(232, 235, 239)"),
    _darkColor("rgb(125, 135, 150)"),
    _borderBackground(0, 74, 158),
    _whiteIsFront(true)
{
    QPalette background(QPalette::Background, _borderBackground);
    setPalette(background);
    _ui->setupUi(this);

    _ui->moveHistoryTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    _ui->moveHistoryTableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    loadPieceSvgs();

    createBoard();
    createImbalanceGrid();
    updateBoard();

    connect(_ui->previousButton, &QPushButton::clicked, this, &Chess::previousPosition);
    connect(_ui->nextButton, &QPushButton::clicked, this, &Chess::nextPosition);
    connect(_ui->moveHistoryTableWidget, &QTableWidget::cellPressed, this, &Chess::selectPosition);

    new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Z), this, SLOT(previousPosition()));
    new QShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_Z), this, SLOT(nextPosition()));
}

void Chess::resizeEvent(QResizeEvent* event) {
    QMainWindow::resizeEvent(event);
    resizeBoard();
}

void Chess::showEvent(QShowEvent* event) {
    QMainWindow::showEvent(event);
    resizeBoard();
}

void Chess::changeEvent(QEvent* event) {
    if(event->type() == QEvent::WindowStateChange) {
        QTimer timer;
        timer.singleShot(1, this, &Chess::resizeBoard);
        event->accept();
    } else {
        QMainWindow::changeEvent(event);
    }
}

void Chess::resizeBoard() {
    int width = _ui->mainHLayout->geometry().width() - _ui->verticalLayout->geometry().width() - 6;
    int height = _ui->mainHLayout->geometry().height();
    int deltaHalf = abs(width - height) / 2;
    int marginX, marginY;
    if(width < height) {
        marginX = width / 40;
        marginY = marginX + deltaHalf;
    } else {
        marginY = height / 40;
        marginX = marginY + deltaHalf;
    }
    _ui->mainHLayout->setContentsMargins(marginX, marginY, marginX, marginY);
}

void Chess::loadPieceSvgs() {
    char pieceColor[] = "LD";
    char pieceType[] = "PNBRQK";

    _pieceSvgContents[0].clear();
    _pieceSvgContents[1].clear();

    for(short colorNo = 0; colorNo < 2; colorNo++) {
        for(short typeNo = 0; typeNo < 6; typeNo++) {
            QString filename = QString("svg/") + QString(pieceType[typeNo]) + QString(pieceColor[colorNo] + QString(".svg"));
            QFile svgFile(filename);
            svgFile.open(QIODevice::ReadOnly);
            _pieceSvgContents[colorNo].push_back(svgFile.readAll());
            svgFile.close();
        }
    }
}

void Chess::createBoard() {
    for(short x = 0; x < 8; x++) {
        _ui->boardGLayout->setColumnStretch(x, 1);
        _ui->boardGLayout->setRowStretch(x, 1);
        for(short y = 0; y < 8; y++) {
            SquareWidget* pieceWidget = new SquareWidget(x, y, this);
            pieceWidget->setAutoFillBackground(true);
            _ui->boardGLayout->addWidget(pieceWidget, convertY(y), x);
            connect(pieceWidget, &SquareWidget::clicked, this, &Chess::clickSquare);
        }
    }
}

void Chess::setStyle(short x, short y, QString squareArgs, QString additionalStyle) {
    if(!Board::positionExists(x, y)) {
        return;
    }
    QString styleSheet = QString(
        "SquareWidget {"
        "   background-color: %1;"
        "   %2"
        "}"
        "%3"
    ).arg((x + y) % 2 ? _lightColor : _darkColor, squareArgs, additionalStyle);
    _ui->boardGLayout->itemAtPosition(convertY(y), x)->widget()->setStyleSheet(styleSheet);
}

void Chess::setBackground(short x, short y) {
    setStyle(x, y, "");
}

void Chess::setBorder(short x, short y, QString borderColor, QString additionalStyle) {
    QString border = QString(
        "   border-style: solid;"
        "   border-color: %1;"
        "   border-width: 5px;"
    ).arg(borderColor);
    setStyle(x, y, border, additionalStyle);
}

void Chess::setMovePosition(short x, short y) {
    QString hover = QString(
        "SquareWidget:hover {"
        "   background-color: #66CC66;"
        "}"
    );
    setBorder(x, y, "#66CC66", hover);
}

void Chess::setCheck() {
    auto checkPosition = _engine.getCheckPosition();
    if(checkPosition.first != -1 && checkPosition.second != -1) {
        setBorder(checkPosition.first, checkPosition.second, "#CC3333");
    }
}

void Chess::updateBackgroundColor() {
    for(short x = 0; x < 8; x++) {
        for(short y = 0; y < 8; y++) {
            setBackground(x, y);
        }
    }
    setCheck();
//    if(_engine.)
}

void Chess::updateBoard() {
    updateBackgroundColor();
    auto board = _engine.getBoard();
    for(short x = 0; x < 8; x++) {
        for(short y = 0; y < 8; y++) {
            SquareWidget* pieceWidget = dynamic_cast<SquareWidget*>(_ui->boardGLayout->itemAtPosition(convertY(y), x)->widget());
            Piece piece = board[x][y].piece;
            if(piece.type == PieceType::None) {
                pieceWidget->load(QByteArray());
                continue;
            }
            size_t typeNo = static_cast<size_t>(piece.type) - 1;
            size_t colorNo = static_cast<size_t>(piece.color);
            pieceWidget->load(_pieceSvgContents[colorNo][typeNo]);
        }
    }
    updateImbalanceGrid();
    updateMoveHistory();
}

void Chess::clearSelection() {
    updateBackgroundColor();
    _engine.deselectPiece();
}

void Chess::updateMoveHistory() {
    auto moveHistory = _engine.getMoveHistory();
    _ui->moveHistoryTableWidget->clearContents();
    if(moveHistory.size() != 0) {
        int row = 0;
        int column = Piece::colorIndex(moveHistory.front().getTurn());
        int rowCount = (moveHistory.size() + 1 + column) / 2;
        _ui->moveHistoryTableWidget->setRowCount(rowCount);
        QStringList rowLabels;
        rowLabels << QString("%1").arg(_engine.getStartingMoveIndex());
        for(auto move : moveHistory) {
            QTableWidgetItem* item = new QTableWidgetItem(QString::fromStdString(move.getAlgebraicNotation()));
            _ui->moveHistoryTableWidget->setItem(row, column, item);
            if(++column == 2) {
                rowLabels << QString("%1").arg(_engine.getStartingMoveIndex() + ++row);
                column = 0;
            }
        }
        _ui->moveHistoryTableWidget->setVerticalHeaderLabels(rowLabels);
    } else {
        _ui->moveHistoryTableWidget->setRowCount(0);
    }
}

void Chess::createImbalanceGrid() {
    clearLayout(_ui->playerMaterialGLayout);
    clearLayout(_ui->enemyMaterialGLayout);
    for(short x = 0; x < 6; x++) {
        for(short y = 0; y < 4; y++) {
            QSvgWidget* frontWidget = new QSvgWidget(this);
            QSvgWidget* backWidget = new QSvgWidget(this);
            frontWidget->setFixedSize(32, 32);
            backWidget->setFixedSize(32, 32);
            _ui->playerMaterialGLayout->addWidget(frontWidget, y, x);
            _ui->enemyMaterialGLayout->addWidget(backWidget, y, x);
        }
    }
}

void Chess::updateImbalanceGrid() {
    auto imbalance = _engine.getMaterialImbalance();

    for(short x = 0; x < 6; x++) {
        for(short y = 0; y < 4; y++) {
            QSvgWidget* frontWidget = dynamic_cast<QSvgWidget*>(_ui->playerMaterialGLayout->itemAtPosition(y, x)->widget());
            QSvgWidget* backWidget = dynamic_cast<QSvgWidget*>(_ui->enemyMaterialGLayout->itemAtPosition(y, x)->widget());
            frontWidget->load(QByteArray());
            backWidget->load(QByteArray());
        }
    }

    short frontRow, frontColumn;
    short backRow, backColumn;
    frontRow = frontColumn = backRow = backColumn = 0;
    for(auto piece : imbalance) {
        QSvgWidget* widget;
        if(piece.color == (_whiteIsFront ? PieceColor::White : PieceColor::Black)) {
            if(frontRow == 4) {
                continue;
            }
            widget = dynamic_cast<QSvgWidget*>(_ui->playerMaterialGLayout->itemAtPosition(frontRow, frontColumn)->widget());
            if(++frontColumn == 6) {
                frontRow++;
                frontColumn = 0;
            }
        } else {
            if(backRow == 4) {
                continue;
            }
            widget = dynamic_cast<QSvgWidget*>(_ui->enemyMaterialGLayout->itemAtPosition(3 - backRow, backColumn)->widget());
            if(++backColumn == 6) {
                backRow++;
                backColumn = 0;
            }
        }
        widget->load(_pieceSvgContents[Piece::colorIndex(piece.color)][static_cast<size_t>(piece.type) - 1]);
    }
}

void Chess::clearLayout(QLayout* layout, bool deleteWidgets)
{
    while (QLayoutItem* item = layout->takeAt(0))
    {
        if (deleteWidgets)
        {
            if (QWidget* widget = item->widget())
                widget->deleteLater();
        }
        if (QLayout* childLayout = item->layout())
            clearLayout(childLayout, deleteWidgets);
        delete item;
    }
}

void Chess::generateBoard(const std::string& fen) {
    try {
    _engine.setBoard(fen);
    updateBoard();
    } catch(std::invalid_argument&) {
        QMessageBox msgBox(this);
        msgBox.setWindowTitle("Error");
        msgBox.setText("Couldn't create a position from the given fen.");
        msgBox.exec();
    }
}

void Chess::showWin(GameState state) {
    VictoryScreen victoryScreen(state, this);
    if(victoryScreen.exec() == VictoryScreen::Accepted) {
        on_actionResetBoard_triggered();
    }
}

void Chess::clickSquare(short x, short y) {
    auto position = std::make_pair(x, y);
    auto selectedSquare = _engine.getSelectedSquare();
    if(selectedSquare) {
        if(selectedSquare->getPosition() == position) {
            clearSelection();
            return;
        } else if(_engine.move(position)) {
            updateBoard();
            auto state = _engine.getGameState();
            if(state != GameState::Playing) {
                showWin(state);
            }
            return;
        }
    }
    if(_engine.getBoard()[x][y].piece.type != PieceType::None) {
        if(_engine.selectPiece(position)) {
            updateBackgroundColor();
            setBorder(x, y, "#FFFF99");
//            _ui->boardGLayout->itemAtPosition(convertY(y), x)->widget()->setStyleSheet("SquareWidget {border-style:solid;border-width:5px;border-color:yellow;}");
//            QPalette selected(QPalette::Foreground, Qt::yellow);
//            _ui->boardGLayout->itemAtPosition(convertY(y), x)->widget()->setPalette(selected);
            for(auto move : _engine.getPossibleMoves()) {
                setMovePosition(move.first, move.second);
            }
        }
    }
}

bool Chess::selectPosition(int move, int color) {
    if(color < 0 || color > 1) {
        return false;
    }
    if(_engine.selectPosition(static_cast<size_t>(2 * move + color + 1))) {
        updateBoard();
        _ui->moveHistoryTableWidget->setCurrentCell(move, color);
        return true;
    } else {
        return false;
    }
}

void Chess::previousPosition() {
    auto positionIndex = _engine.previousPosition() - 1;
    updateBoard();
    if(positionIndex != -1) {
        _ui->moveHistoryTableWidget->setCurrentCell(positionIndex / 2, positionIndex % 2);
    }
}

void Chess::nextPosition() {
    auto positionIndex = _engine.nextPosition() - 1;
    updateBoard();
    if(positionIndex != -1) {
        _ui->moveHistoryTableWidget->setCurrentCell(positionIndex / 2, positionIndex % 2);
    }
}

short Chess::convertY(short y) {
    if(_whiteIsFront) {
        return 7 - y;
    } else {
        return y;
    }
}

Chess::~Chess() {
    delete _ui;
}

void Chess::on_actionBoardGeneration_triggered() {
    QInputDialog inputBox(this);
    inputBox.setInputMode(QInputDialog::TextInput);
    inputBox.setWindowModality(Qt::ApplicationModal);
    inputBox.setWindowTitle("Generator");
    inputBox.setLabelText("Please paste fen in this area");
    if(inputBox.exec() == QInputDialog::Accepted) {
        generateBoard(inputBox.textValue().toStdString());
    }

}

void Chess::on_actionAboutCreator_triggered() {
    QMessageBox* msgBox = new QMessageBox(this);
    msgBox->setModal(false);
    msgBox->setWindowTitle("About creator");
    msgBox->setText("This program was created by Patryk Chodorowski");
    msgBox->show();
}

void Chess::on_actionCommands_triggered() {
    QMessageBox* msgBox = new QMessageBox(this);
    msgBox->setModal(false);
    msgBox->setWindowTitle("Shortcuts");
    msgBox->setText(QString("Ctrl + Z:\t\tUndo Move\n") +
                   QString("Ctrl + Shift + Z:\tRedo Move"));
    msgBox->show();
}

void Chess::on_actionResetBoard_triggered() {
    generateBoard("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq -");
}
