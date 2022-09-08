#include "window.h"
#include "glwidget.h"

#include <QGridLayout>
#include <QLabel>
#include <QPushButton>

Window::Window()
{
    GLWidget* openGL = new GLWidget(this);
    QPushButton *button = new QPushButton(this);
    button->setText("Push me");
    QGridLayout* layout = new QGridLayout(this);

    QKeyEvent* event;

    openGL->hasFocus();
    layout->addWidget(openGL, 0, 0);
    layout->addWidget(button, 0, 1);
    setLayout(layout);
}
