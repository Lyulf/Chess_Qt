#include "chess.hpp"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Chess w;
    w.show();

    return a.exec();
}
