#ifndef WINDOW_H
#define WINDOW_H

#include <QWidget>

//! [0]
class Window : public QWidget
{
    Q_OBJECT

public:
    Window();
    //void keyPressEvent(QKeyEvent* event) override;

protected:
    void keyPressEvent(QKeyEvent* event) override;

private:
};

#endif // WINDOW_H
