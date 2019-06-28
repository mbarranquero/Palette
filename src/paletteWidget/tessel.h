#ifndef TESSEL_H
#define TESSEL_H
#include "dish.h"
#include <QPoint>
#include <QColor>

class Tessel
{
public:
    Tessel(Dish * _dish, QPoint _point);
    Tessel(){}
    QColor getColor();

private :
    Dish * dish = nullptr;
    QPoint point = QPoint(0,0);
};

#endif // TESSEL_H
