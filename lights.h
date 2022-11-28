#ifndef LIGHTS_H
#define LIGHTS_H

#include <stdint.h>
#include <QApplication>
#include <QSerialPort>
#include <QSerialPortInfo>

enum Weather{
    CLEAR = 0,
    RAIN = 1,
    CLOUDS = 2,
};


class LightManager : public QObject {
    Q_OBJECT

    QSerialPort* uno;

    int vid = 9025;
    int pid = 67;
    int numLeds = 30;
    bool alternating = false;

public:
    LightManager();

public slots:
    void mirrorScreen(QImage* screen);
    void mirrorGhosts(int dist);
};



#endif // LIGHTS_H0.............................
