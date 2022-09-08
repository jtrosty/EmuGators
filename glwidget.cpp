#include "glwidget.h"


GLWidget::GLWidget(QWidget* parent) : QOpenGLWidget(parent) {
    //setGeometry(20,20, 256, 240);
    setFixedSize(width * scale, height * scale);
    setMinimumSize(width * scale, height * scale);
    setAutoFillBackground(false);
    GLWidget::setEnabled(true);
    GLWidget::grabKeyboard();

    pixelData = new uchar[pixelDataLength];
    for (int i = 0; i < pixelDataLength; i += 4) {
        pixelData[i] 	 = startB++;
        pixelData[i + 1] = startG--;
        pixelData[i + 2] = startR++;
        pixelData[i + 3] = startA;
    }
    imageData = new QImage(pixelData, width * scale, height * scale, QImage::Format_ARGB32);
    //pixmap = new QPixmap(width * scale, height * scale);
    //pixmap->loadFromData(pixelData, pixelDataLength);
}

GLWidget::~GLWidget() {
    delete[] pixelData;
}

// TODO (Jon) - probably delte this.
void GLWidget::changeInputX(bool direction) {
    if (direction)  inputX += 10;
    else 			inputX -= 10;
}

void GLWidget::updatePixelData() {
    for (int i = 0; i < (width * height * scale * bytesPerPixel); i += 4) {
        pixelData[i] 	 += inputX;
        pixelData[i + 1] += inputY;
    }
    for (int i = 0; i < imageData->height(); i++) {
        for (int j = 0; j < imageData->width(); j++) {
        }
    }
}

void GLWidget::initializeGL() {
/*
// Set up the rendering context, load shaders and other resources, etc.:
QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
f->glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

glEnable(GL_DEBUG_OUTPUT);
*/
}

void GLWidget::paintGL() {
    updatePixelData();
    QPainter p(this);
    //delete imageData;
    //imageData = new QImage(pixelData, width * scale, height * scale, QImage::Format_ARGB32);
    /*
    if(pixmap->loadFromData(pixelData, pixelDataLength)) {

        //great
    }
    else {

    }
    */
    imageData->loadFromData(pixelData, pixelDataLength);
    //p.drawPixmap(QPoint(0,0), *pixmap);
    p.drawImage(QPoint(0,0), *imageData);
    /*
    p.setPen(Qt::red);
    p.drawLine(rect().topLeft(), rect().bottomRight());
    */
}

void GLWidget::resizeGL(int w, int h) {
    glViewport(0, 0, w, h);
}

void GLWidget::keyPressEvent(QKeyEvent *event) {
    switch (event->key()) {
    case Qt::Key_W:
        inputX += 10;
        test_W = true;
        this->update();
        break;
    case Qt::Key_S:
        inputX += -10;
        test_W = false;
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
