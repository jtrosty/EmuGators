#ifndef WINDOW_H
#define WINDOW_H

#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>

#include "defs.h"
#include "glwidget.h"
#include "romloader.h"

//! [0]
class Window : public QWidget
{
    Q_OBJECT

public:
    Window(u32* _pixelData);
    GLWidget* openGL;

protected:

private:
};

#endif // WINDOW_H
