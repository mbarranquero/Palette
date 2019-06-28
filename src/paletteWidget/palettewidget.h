#ifndef PALETTEWIDGET_H
#define PALETTEWIDGET_H
#include <QSlider>
#include <QWidget>
#include <QGroupBox>
#include <QBoxLayout>

class PaletteWidget : public QWidget
{
    Q_OBJECT

    public:
        explicit PaletteWidget(QWidget *parent = 0);
        ~PaletteWidget();

    private:
        QGroupBox * buttonZone;
};

#endif // PALETTEWIDGET_H
