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

public:
    LightManager();

public slots:
    void setMode(int mode);
    void setMode();
};



#endif // LIGHTS_H0.............................
