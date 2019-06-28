#ifndef DISHTOOLS_H
#define DISHTOOLS_H

#include <QSlider>
#include <QGroupBox>
#include <QBoxLayout>
#include <QPushButton>
#include <QSlider>
#include "canvas.h"
#include "blob.h"
#include "tessel.h"

class DishTools : public QGroupBox
{
    Q_OBJECT

public:
    DishTools(Canvas * canv);
    //QWidget * getColorSelected(){return colorSelected;};

private:
    Canvas * canvas = nullptr;
    QSlider * sizePicker = nullptr;
    QColor runningColor = QColor(230, 230, 230);
    QWidget * colorSelected = nullptr;
    QWidget * pickedColor = nullptr;
    Tessel * currentTessel = nullptr;

public slots:
    void setBlobSize(int p);
    void setBlobColor();
    void setDefaultValues(Blob * b );
    void addBlob();
    void paintEvent(QPaintEvent*);
    void blobColorChange(Blob* b);
    void setRunningColor(QColor c) {runningColor = c; update();}
    void setCurrentTessel(Tessel * c) {currentTessel = c; update();}

};

#endif // DISHTOOLS_H
