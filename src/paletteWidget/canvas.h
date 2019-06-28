#ifndef CANVAS_H
#define CANVAS_H
#include "dish.h"
#include <QWidget>
#include <QMouseEvent>
#include <QState>
#include <QCursor>
#include "blob.h"
#include "tessel.h"

class Canvas : public QWidget
{
    Q_OBJECT

private :
    Dish * dish;
    int canvasWidth = 300;
    int canvasHeight = 300;
    bool moveIsOn = false;
    Blob * selectedBlobMove = nullptr;
    Blob * selectedBlobEdit = nullptr;
    bool pickedModeIsOn = false;
    QState * passiveState = nullptr;
    QState * moveBlob = nullptr;
    QState * editBlob = nullptr;
    QStateMachine * mac = nullptr;
    QPoint currentPos = QPoint(0,0);

public:
    explicit Canvas(QWidget *parent = 0);
    Blob * getSelectedBlobMove(){
        return selectedBlobMove;
    }
    Blob * getSelectedBlobEdit(){
        return selectedBlobEdit;
    }

    Dish * getDish() {
        return dish;
    }

    void setSelectedBlobEdit(Blob * b) {selectedBlobEdit = b;}

signals:
    void dragOn(Blob * b);
    void dragOff();
    void doubleClickOn(Blob * b);
    void doubleClickOff();
    void tesselPicked(Tessel * t);


public slots:
    void paintEvent(QPaintEvent*);
    void setPickedModeOn(){pickedModeIsOn = true; selectedBlobEdit = nullptr; setCursor(QCursor(Qt::BlankCursor)); update();}
    void setPickedModeOff(){pickedModeIsOn = false; setCursor(QCursor(Qt::ArrowCursor)); update();}

protected:
    void mousePressEvent(QMouseEvent * e);
    void mouseMoveEvent(QMouseEvent * e);
    void mouseReleaseEvent(QMouseEvent * e);
    void mouseDoubleClickEvent(QMouseEvent * e);
};

#endif // CANVAS_H
