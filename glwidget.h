 #ifndef GLWIDGET_H
#define GLWIDGET_H

#include "weather.h"
#include "lights.h"
#include <QWidget>
#include <QOpenGLWidget>
#include <QPainter>
#include <QOpenGLFunctions>
#include <QImage>
#include <QPoint>
#include <QKeyEvent>

class GLWidget : public QOpenGLWidget
{
    Q_OBJECT
public:
    // VARIABLES
    uchar* pixelData;
    uchar startR = 0xFF;
    uchar startB = 0x00;
    uchar startG = 0x00;
    uchar startA = 0xFF;

    int width = 256;
    int height = 240;
    int scale = 2;
    int bytesPerPixel = 4;
    int widthScaled = width * scale;
    int heightScaled = height * scale;
    int bytesPerLine = width * bytesPerPixel;
    uchar inputX = 0;
    uchar inputY = 0;
    uint pixelDataLength = width * height * bytesPerPixel;

    WeatherManager* weather = nullptr;
    LightManager* lights = nullptr;

    GLWidget(QWidget* parent);

    ~GLWidget();

    void keyPressEvent(QKeyEvent* event) override;

    // Slots

    // Signals

    // Helper functions
    void debug_updatePixelData();

    // Protected virtual functions for OpenGL, we do not call these
    protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;
};

#endif // GLWIDGET_H
