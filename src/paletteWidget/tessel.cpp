#include "tessel.h"

Tessel::Tessel(Dish* d, QPoint p)
{
    dish = d;
    point = p;
}

QColor Tessel::getColor() {
    return QColor(dish->pixel(point));
}
