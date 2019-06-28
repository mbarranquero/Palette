#ifndef BLOB_H
#define BLOB_H
#include <QPoint>
#include <QColor>
#include <QPushButton>

class Blob
{
private :
    QColor color;
    QPoint center;
    unsigned int radius;
    //QPushButton selectButton;

public:
    Blob(QColor c, QPoint p, int r) {
        color = c;
        center = p;
        radius = r;
    }
    inline const QColor getColor() {return color;}
    inline const QPoint getCenter() {return center;}
    inline unsigned int getRadius() {return radius;}
    inline void setColor(QColor c) {color = c;}
    inline void setCenter(QPoint p) {center = p;}
    inline void setRadius(unsigned int r) {radius = r;}
};

#endif // BLOB_H
