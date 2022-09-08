#include "window.h"
#include "glwidget.h"

#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QKeyEvent>

Window::Window()
{
    GLWidget* openGL = new GLWidget(this);
    QPushButton *button = new QPushButton(this);
    button->setText("Push me");
    QWidget* layoutWidget = new QWidget(this);
    QGridLayout* layout = new QGridLayout(this);
    //layoutWidget->setGeometry(20,20, 500, 300);

    QKeyEvent* event;

    openGL->hasFocus();
    layout->addWidget(openGL, 0, 0);
    layout->addWidget(button, 0, 1);
    setLayout(layout);
    QLabel* test_label = new QLabel(this);
    test_label->setGeometry(50, 50, 500, 500);
    if (openGL->test_W) {
        test_label->setText("Pressed W");
    }
    else {
        test_label->setText("No press W");
    }


    /*
    if (event->key() == Qt::Key_F) {
    }
    */

}

void Window::keyPressEvent(QKeyEvent* event)
{

    if(event->key() == Qt::Key_W)
    {
    }

    if(event->key() == Qt::Key_S)
    {
    }

    if(event->key() == Qt::Key_A)
    {
    }

    if(event->key() == Qt::Key_D)
    {
    }
}

/*
void Window::keyPressEvent(QKeyEvent *event) {
    switch (event->key()) {
    case Qt::Key_W:
        openGL->inputX = 256;
        openGL->test_W = true;
        this->update();
        break;
    case Qt::Key_S:
        openGL->inputX = 0;
        openGL->test_W = false;
        this->update();
        break;
    case Qt::Key_Escape:
        this->update();
        break;
    default:
        event->ignore();
        break;
    }
}

*/
