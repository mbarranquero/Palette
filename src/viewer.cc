#include <iostream>
#include <QApplication>
#include <QSurfaceFormat>
#include <QOpenGLContext>

#include "window/mainwindow.hh"


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QSurfaceFormat fmt;
    fmt.setDepthBufferSize(24);

    // Request OpenGL 4.5 compatibility
    qDebug("Requesting 4.5 Core context");
    fmt.setVersion(4, 5);
    fmt.setProfile(QSurfaceFormat::CoreProfile);

    QSurfaceFormat::setDefaultFormat(fmt);

    MainWindow window;
    window.show();

    return app.exec();
}
