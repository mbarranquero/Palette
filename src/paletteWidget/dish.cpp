#include "dish.h"
#include <tuple>
#include <math.h>
#include <QVector3D>


float metaball(float d2, float b) {
    if (d2<=(b*b)) {
        return (1 - 4*pow(d2,3)/(9*pow(b,6)) + 17*pow(d2,2)/(9*pow(b,4)) - 22*d2/(9*pow(b,2)));
    }
    else {
        return 0;
    }
}


void Dish::pixelShader(QPoint p) {

    QRgb background = qRgb(254, 253, 240);
    colorTable().push_back(background);
    unsigned int n = blobs.size();
    QVector3D rgb = QVector3D(0,0,0);

    float sum = 0;
    for (unsigned int i =0; i < n; i++) {

        QColor blobColor = blobs[i]->getColor();
        QVector3D c = QVector3D(blobColor.redF(), blobColor.greenF(), blobColor.blueF());

        QPoint diff = p -  blobs[i]->getCenter();
        float d2 = pow(diff.x(),2) +pow(diff.y(),2);
        float intensity = metaball(d2,(float) blobs[i]->getRadius());
        sum += intensity;
        rgb = rgb + c * intensity;

    }

    if(sum > treshold) {
        rgb = rgb/sum;
        QRgb value = qRgb((int) 255*rgb.x(), (int) 255*rgb.y(), (int) 255*rgb.z());
        colorTable().push_back(value);
        setPixel(p, value);

    }
    else {
        setPixel(p, background);
    }
}

void Dish::paint() {
    for (int y = 0; y < height(); y++) {
        for (int x = 0; x < width(); x++) {
            pixelShader(QPoint(x,y));
        }
    }
}
