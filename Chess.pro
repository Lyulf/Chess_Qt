#-------------------------------------------------
#
# Project created by QtCreator 2019-02-13T13:56:40
#
#-------------------------------------------------

QT       += core gui svg

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Chess
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
    src/board.cpp \
    src/game.cpp \
    src/game_rules.cpp \
    src/move_info.cpp \
    src/piece.cpp \
    src/pressure_factory.cpp \
    src/square.cpp \
    src/string_tok.cpp \
    src/engine.cpp \
    src/square_widget.cpp \
    main.cpp \
    chess.cpp \
    src/victory_screen.cpp

HEADERS += \
    include/chess/engine/board.hpp \
    include/chess/engine/game.hpp \
    include/chess/engine/game_rules.hpp \
    include/chess/engine/move_info.hpp \
    include/chess/engine/move_type.hpp \
    include/chess/engine/piece.hpp \
    include/chess/engine/pressure_factory.hpp \
    include/chess/engine/square.hpp \
    include/chess/engine/string_tok.hpp \
    include/chess/engine/engine.hpp \
    include/chess/square_widget.hpp \
    chess.hpp \
    include/chess/victory_screen.hpp \
    include/chess/engine/game_state.hpp

FORMS += \
        chess.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
