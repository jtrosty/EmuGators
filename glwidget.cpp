#include "glwidget.h"

GLWidget::GLWidget(QWidget* parent, u32* _pixelData) : QOpenGLWidget(parent) {
    //setGeometry(20,20, 256, 240);
    //setFixedSize(widthScaled, heightScaled);
    //setMinimumSize(width * scale, height * scale);
    //setMinimumSize(widthScaled, heightScaled);
    setMinimumSize(1024, 1024);
    setAutoFillBackground(false);
    GLWidget::setEnabled(true);
    GLWidget::grabKeyboard();

    pixelData = _pixelData;

    //_pixelData = new u32[pixelDataLength / 4];
    /*
    uchar* tempPixelData = (uchar*)&_pixelData;
    for (int i = 0; i < pixelDataLength; i += 4) {
        tempPixelData[i] 	 = startB;
        tempPixelData[i + 1] = startG++;
        tempPixelData[i + 2] = startR--;
        tempPixelData[i + 3] = startA;
    }
    */
}

GLWidget::~GLWidget() {
    delete[] pixelData;
}

void GLWidget::keyPressEvent(QKeyEvent *event) {
    switch (event->key()) {
    case Qt::Key_W:
        inputY = 10;
        this->update();
        break;
    case Qt::Key_S:
        inputY = -10;
        this->update();
        break;
    case Qt::Key_A:
        inputX = 10;
        this->update();
        break;
    case Qt::Key_D:
        inputX = -10;
        this->update();
        break;
    case Qt::Key_Space:
        this->update();
        break;
    case Qt::Key_F:
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

void GLWidget::debug_updatePixelData() {
    for (int i = 0; i < pixelDataLength/4; i++ ) {
        pixelData[i] 	 += inputX;
        //pixelData[i + 1] += inputY;
    }
    inputX = 0;
    inputY = 0;
}

void GLWidget::initializeGL() {
    // Set up the rendering context, load shaders and other resources, etc.:
    // NOTE (Jon): I'm  not entirely sure if we need the next 2 lines, the OpenGLFunctions.
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    f->glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    glEnable(GL_DEBUG_OUTPUT);
}

void GLWidget::paintGL() {
    //QOpenGLFunctions* f = QOpenGLContext::currentContext()->functions();
    //f->glClear(GL_COLOR_BUFFER_BIT);
    //glClear(GL_COLOR_BUFFER_BIT);
    //debug_updatePixelData();
    QPainter p(this);
    //QImage img((const uchar *)(pixelData), width, height, bytesPerLine, QImage::Format_ARGB32);
    QImage img((const uchar *)(pixelData), 128, 128, (128 * 4), QImage::Format_ARGB32);

    //weather->addWeatherEffect(&img);

    p.drawImage(rect(), img);
}

void GLWidget::resizeGL(int w, int h) {
    glViewport(0, 0, w, h);
}
