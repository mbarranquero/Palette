#include "dishtools.h"
#include <QColorDialog>
#include <QLabel>

DishTools::DishTools(Canvas * canv) : QGroupBox("Outils")
{
    canvas = canv;

    colorSelected = new QWidget();
    QPushButton * colorPicker = new QPushButton("Color Picker");
    QPushButton * newBlob = new QPushButton("Add Blob");
    QPushButton * blobColorPicker = new QPushButton("Blob Color");
    sizePicker = new QSlider(Qt::Horizontal);
    colorSelected->setMaximumHeight(50);
    QPalette pal = palette();
    pal.setColor(QPalette::Background, runningColor);
    colorSelected->setAutoFillBackground(true);
    colorSelected->setPalette(pal);

    pickedColor = new QWidget();
    pickedColor->setMaximumHeight(50);
    pal = palette();
    pal.setColor(QPalette::Background, QColor(254, 253, 240));
    pickedColor->setAutoFillBackground(true);
    pickedColor->setPalette(pal);

    QVBoxLayout * sideButtons = new QVBoxLayout();
    sideButtons->addWidget(new QLabel("picked color : "));
    sideButtons->addWidget(pickedColor);
    sideButtons->addWidget(colorPicker);
    sideButtons->addWidget(new QLabel("blob color : "));
    sideButtons->addWidget(colorSelected);
    sideButtons->addWidget(blobColorPicker);
    sideButtons->addWidget(sizePicker);
    sideButtons->addWidget(newBlob);


    sizePicker->setMinimum(15);
    sizePicker->setMaximum(150);
    sizePicker->setValue(62);

    connect(colorPicker, SIGNAL(clicked()), canvas, SLOT(setPickedModeOn()));
    connect(blobColorPicker, SIGNAL(clicked()), this, SLOT(setBlobColor()));
    connect(sizePicker, SIGNAL(sliderMoved(int)), this, SLOT(setBlobSize(int)));
    connect(canvas, SIGNAL(doubleClickOn(Blob*)), this, SLOT(setDefaultValues(Blob *)));
    connect(newBlob, SIGNAL(clicked()), this, SLOT(addBlob()));
    connect(canvas,SIGNAL(doubleClickOn(Blob*)),this, SLOT(blobColorChange(Blob*)));
    connect(canvas, SIGNAL(tesselPicked(Tessel*)), this, SLOT(setCurrentTessel(Tessel*)));
    setLayout(sideButtons);
}

void DishTools::setBlobSize(int ray){
    Blob * b = canvas->getSelectedBlobEdit();
    if (b) {
        b->setRadius(ray);
        canvas->update();
    }
}

void DishTools::setDefaultValues(Blob * b ){
    if (b) {
        sizePicker->setValue(b->getRadius());
    }
}

void DishTools::setBlobColor(){
    Blob * b = canvas->getSelectedBlobEdit();
    if (b) {
        QPalette pal = palette();
        QColor color = QColorDialog::getColor(b->getColor(), nullptr);
        setRunningColor(color);
        b->setColor(runningColor);
        pal.setColor(QPalette::Background, runningColor);
        colorSelected->setAutoFillBackground(true);
        colorSelected->setPalette(pal);

    }
    else{
        QColor color = QColorDialog::getColor(runningColor, nullptr);
        setRunningColor(color);
        QPalette pal = palette();
        pal.setColor(QPalette::Background, runningColor);
        colorSelected->setAutoFillBackground(true);
        colorSelected->setPalette(pal);

    }
    canvas->setPickedModeOff();
    canvas->update();
}

void DishTools::addBlob() {
    Blob * b = new Blob(runningColor,
                   QPoint(50, 50),
                   50);

    b->setRadius(sizePicker->value());
    canvas->getDish()->addBlob(b);
    canvas->setSelectedBlobEdit(b);
    canvas->setPickedModeOff();
    canvas->update();
}

void DishTools::paintEvent(QPaintEvent* e) {
    QPalette pal = palette();
    pal.setColor(QPalette::Background, runningColor);
    colorSelected->setAutoFillBackground(true);
    colorSelected->setPalette(pal);

    QPalette pal2 = palette();

    if(currentTessel){
        QColor c = currentTessel->getColor();
        pal2.setColor(QPalette::Background, c);
        pickedColor->setAutoFillBackground(true);
        pickedColor->setPalette(pal2);
    }
}

void DishTools::blobColorChange(Blob *b) {
    QColor c = b->getColor();
    setRunningColor(c);
}
