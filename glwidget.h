 #ifndef GLWIDGET_H
#define GLWIDGET_H

<<<<<<< HEAD
=======
#include "weather.h"
#include "lights.h"
>>>>>>> gabe2
#include <QWidget>
#include <QOpenGLWidget>
#include <QPainter>
#include <QOpenGLFunctions>
#include <QImage>
#include <QPoint>
#include <QKeyEvent>
#include <QByteArray>

#include "weather.h"
#include "defs.h"

class GLWidget : public QOpenGLWidget
{
    Q_OBJECT
public:
    // VARIABLES
    //uchar* pixelData;
    u32* pixelData;
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

    GLWidget(QWidget* parent, u32* _pixelData);

    ~GLWidget();

    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;

    // Slots

    // Signals

    // Helper functions
    void debug_updatePixelData();
    QByteArray getPixelData();

    // Protected virtual functions for OpenGL, we do not call these
    protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;
private:
    void keyPressImpl(QKeyEvent*, u8 (*addOrRemoveKey)(int, u8));
    void debugKeyPressEvent(QKeyEvent* event);
    void debugKeyReleaseEvent(QKeyEvent* event);

};

#endif // GLWIDGET_H
