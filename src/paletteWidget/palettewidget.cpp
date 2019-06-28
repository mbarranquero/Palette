#include "palettewidget.h"
#include "blob.h"
#include "dish.h"
#include "canvas.h"
#include "dishtools.h"


PaletteWidget::PaletteWidget(QWidget *parent) :
    QWidget(parent)
{
    Canvas * canva = new Canvas(this);

    buttonZone = new DishTools(canva);
    //setCentralWidget(new Canvas(this));

    QHBoxLayout * mainLayout = new QHBoxLayout(this);

    mainLayout->addWidget(buttonZone);
    mainLayout->addWidget(canva);


}

PaletteWidget::~PaletteWidget()
{

}
