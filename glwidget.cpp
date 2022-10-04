#include "glwidget.h"

GLWidget::GLWidget(QWidget* parent) : QOpenGLWidget(parent) {
    //setGeometry(20,20, 256, 240);
    //setFixedSize(widthScaled, heightScaled);
    //setMinimumSize(width * scale, height * scale);
    setMinimumSize(widthScaled, heightScaled);
    setAutoFillBackground(false);
    GLWidget::setEnabled(true);
    GLWidget::grabKeyboard();

    pixelData = new uchar[pixelDataLength];
    for (int i = 0; i < pixelDataLength; i += 4) {
        pixelData[i] 	 = startB;
        pixelData[i + 1] = startG++;
        pixelData[i + 2] = startR--;
        pixelData[i + 3] = startA;
    }
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
    for (int i = 0; i < pixelDataLength; i += 4) {
        pixelData[i] 	 += inputX;
        pixelData[i + 1] += inputY;
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
    debug_updatePixelData();
    QPainter p(this);
    QImage img((const uchar *)(pixelData), width, height, bytesPerLine, QImage::Format_ARGB32);

    weather->addWeatherEffect(&img);

    p.drawImage(rect(), img);


}

void GLWidget::resizeGL(int w, int h) {
    glViewport(0, 0, w, h);
}
