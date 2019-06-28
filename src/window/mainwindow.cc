#include "mainwindow.hh"
#include <QCheckBox>
#include <QToolBar>
#include <QWidget>
#include <QBoxLayout>
#include "../paletteWidget/palettewidget.h"

MainWindow::MainWindow()
{

    QWidget *cw = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(cw);
    QHBoxLayout *toolLayout = new QHBoxLayout();
    palette = new PaletteWidget(this);
    toolLayout->addWidget(palette);
    toolLayout->addStretch(5);

    gl_view_ = new GLView(this);
    mainLayout->addWidget(gl_view_);
    mainLayout->addLayout(toolLayout);
    setCentralWidget(cw);

    QToolBar *tool_bar = new QToolBar("Tools", this);
    addToolBar(Qt::LeftToolBarArea, tool_bar);

    // Setup CheckBox for toggle pbr/color map rendering
    QCheckBox *checkbox = new QCheckBox("Only Color Map", tool_bar);
    tool_bar->addWidget(checkbox);
    connect(checkbox, SIGNAL(toggled(bool)), gl_view_, SLOT(toggleColorMap(bool)));
}
