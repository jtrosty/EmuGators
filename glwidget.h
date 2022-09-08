#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QWidget>
#include <QOpenGLWidget>
#include <QPainter>
#include <QOpenGLFunctions>
#include <QPixmap>//TODO(Jon) delete?
#include <QImage>
#include <QPoint>
#include <QKeyEvent>
#include <QPixmap>

class GLWidget : public QOpenGLWidget
{
    Q_OBJECT
public:
    // VARIABLES
    uchar* pixelData;
    QImage* imageData;
    QPixmap* pixmap;
    uchar startR = 0x88;
    uchar startB = 0x00;
    uchar startG = 0xFF;
    uchar startA = 0xFF;

    int width = 256;
    int height = 240;
    int scale = 2;
    int bytesPerPixel = 4;
    uchar inputX = 0;
    uchar inputY = 0;
    uint pixelDataLength = width * height * scale * bytesPerPixel;

    bool test_W = false;

    GLWidget(QWidget* parent);

    ~GLWidget();

    void keyPressEvent(QKeyEvent* event) override;

    // Helper functions
    void changeInputX(bool direction);
    void updatePixelData();

    // Protected virtual functions for OpenGL, we do not call these
    protected:
    void initializeGL() override;
    void paintGL() override;

    void resizeGL(int w, int h) override;
    //void paintEvent(QPaintEvent* event) override;
};

#endif // GLWIDGET_H
