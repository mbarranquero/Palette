#include "canvas.h"
#include <QPainter>
#include <QRect>
#include <QPoint>
#include <QStateMachine>
#include <QBrush>
#include <QPen>
#include "dishtools.h"
#include "tessel.h"

Canvas::Canvas(QWidget *parent) : QWidget(parent)
{
//    mac = new QStateMachine();

//    passiveState = new QState(mac);
//    moveBlob = new QState(mac);
//    editBlob = new QState(mac);

//    passiveState->addTransition(this,SIGNAL(dragOn(Blob*)),moveBlob);
//    passiveState->addTransition(this, SIGNAL(doubleClickOn(Blob*)), editBlob);
//    moveBlob->addTransition(this,SIGNAL(dragOff()),passiveState);
//    editBlob->addTransition(this, SIGNAL(doubleClickOff()), passiveState);

    setMinimumSize(canvasWidth,canvasHeight);
    std::vector<Blob*> blobs= std::vector<Blob*>();
    dish = new Dish(canvasWidth, canvasHeight, blobs);
    setMouseTracking(true);
    dish->paint();

}



void Canvas::mousePressEvent(QMouseEvent * e){

    QRect clickRect = QRect((e->pos()), QSize(3,3));
    selectedBlobMove = nullptr;
    if (!pickedModeIsOn) {
        for(unsigned int i = 0; i < dish->getBlobs().size(); i++){
            float rad = dish->getBlobs()[i]->getRadius();
            float l = 3*rad/4;
            QRect blobRect = QRect(dish->getBlobs()[i]->getCenter()-QPoint(0.7*l*2/3,0.7*l*2/3), QSize(l,l));
            if(blobRect.intersects(clickRect)){
                moveIsOn = true;
                selectedBlobMove = dish->getBlobs()[i];
                emit dragOn(selectedBlobMove);
                break;
            }
        }
    }

//    else {
//        currentPos = e->pos();
//        emit tesselPicked(new Tessel(dish, e->pos()));
//    }
    update();
}

void Canvas::mouseMoveEvent(QMouseEvent * e){

    if (pickedModeIsOn) {
        currentPos =  this->mapFromGlobal(QCursor::pos());
        emit tesselPicked(new Tessel(dish, e->pos()));
    }

    else if (selectedBlobMove && moveIsOn && (e->buttons() & Qt::LeftButton) ) {
        selectedBlobMove->setCenter(e->pos());
        setCursor(QCursor(Qt::ClosedHandCursor));

    }

    update();
}

void Canvas::mouseReleaseEvent(QMouseEvent * e){

    if(moveIsOn) {
        selectedBlobMove = nullptr;
        emit dragOff();
        moveIsOn = false;
        setCursor(QCursor(Qt::ArrowCursor));
    }
    update();

}

void Canvas::mouseDoubleClickEvent(QMouseEvent * e){

    QRect clickRect = QRect((e->pos()), QSize(3,3));
    selectedBlobEdit = nullptr;
    setPickedModeOff();
    if (!pickedModeIsOn) {
        for(unsigned int i = 0; i < dish->getBlobs().size(); i++){
            float rad = dish->getBlobs()[i]->getRadius();
            float l = 3*rad/4;
            QRect blobRect = QRect(dish->getBlobs()[i]->getCenter()-QPoint(0.7*l*2/3,0.7*l*2/3), QSize(l,l));
            if(blobRect.intersects(clickRect)){
                selectedBlobEdit = dish->getBlobs()[i];

                emit doubleClickOn(selectedBlobEdit);
                break;
            }
        }

        if (!selectedBlobEdit) {
            emit doubleClickOff();
        }
    }
    else {
        setPickedModeOff();
    }

    update();
}

void Canvas::paintEvent(QPaintEvent*e) {
    dish->paint();
    QPainter p;
    p.begin(this);
    p.drawImage(QRect(0, 0,canvasWidth, canvasHeight), *dish);
    if (selectedBlobEdit) {
        p.setBrush(Qt::black);
        p.drawEllipse(QRect((selectedBlobEdit->getCenter())-QPoint(15/4,15/4), QSize(5,5)));
    }
    if(pickedModeIsOn){
        p.setPen(Qt::black);
        p.setBrush(QBrush(QColor(dish->pixel(currentPos))));
        float l = 3*20/4;
        p.drawEllipse(QRect(currentPos-QPoint(0.7*l*2/3,0.7*l*2/3), QSize(20, 20)));
    }

    p.end();
}
