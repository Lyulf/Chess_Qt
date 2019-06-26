#ifndef SQUARE_WIDGET_HPP
#define SQUARE_WIDGET_HPP

#include <QSvgWidget>

class SquareWidget : public QSvgWidget {
    Q_OBJECT

public:
    SquareWidget(short x, short y, QWidget* parent = nullptr);

protected slots:
    void mousePressEvent(QMouseEvent *event) override;

signals:
    void clicked(short x, short y);

private:
    short _x, _y;
};

#endif // SQUARE_WIDGET_HPP
