#ifndef DISH_H
#define DISH_H
#include "blob.h"
#include <QColor>
#include <QImage>
#include <iostream>

class Dish : public QImage
{
private :
    std::vector<Blob*> blobs= std::vector<Blob*>();
    float treshold = 0.45;
    void pixelShader(QPoint p);

public :

    Dish() : QImage(QSize(200, 200), QImage::Format_RGB16) {}
    Dish(int height, int width, std::vector<Blob*> b) : QImage(QSize(height, width), QImage::Format_RGB16) {
        blobs = b;
    }

    void addBlob(Blob * b) {
        blobs.push_back(b);
    }

    std::vector<Blob*> getBlobs(){
        return blobs;
    }

    void paint();
};

#endif // DISH_H
