#pragma once

#include <QMainWindow>
#include "glview.hh"
#include "../paletteWidget/palettewidget.h"

class MainWindow : public QMainWindow
{
public:
    MainWindow();

private:
    GLView *gl_view_;
    PaletteWidget *palette;
};
