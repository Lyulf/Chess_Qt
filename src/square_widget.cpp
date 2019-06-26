#include "../include/chess/square_widget.hpp"

SquareWidget::SquareWidget(short x, short y, QWidget* parent)
    : QSvgWidget(parent), _x(x), _y(y) {
    setMouseTracking(true);
}

void SquareWidget::mousePressEvent(QMouseEvent* event) {
    QSvgWidget::mousePressEvent(event);
    emit clicked(_x, _y);
}
