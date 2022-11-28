#include "lights.h"

LightManager::LightManager(){

    QString portName;
    bool available = false;

    for(const QSerialPortInfo &serialPortInfo : QSerialPortInfo::availablePorts()){
        if(serialPortInfo.hasVendorIdentifier() && serialPortInfo.hasProductIdentifier()){
            if(serialPortInfo.vendorIdentifier() == vid){
                if(serialPortInfo.productIdentifier() == pid){
                    portName = serialPortInfo.portName();
                    available = true;
                }
            }
        }
    }
    uno = new QSerialPort(portName);

    if(available){
        // open and configure the serialport
        uno->setPortName(portName);
        if(!uno->open(QSerialPort::WriteOnly)){
            qDebug() << uno->error();
        }
        uno->setBaudRate(QSerialPort::Baud19200);
        uno->setDataTerminalReady(true);
        uno->setDataBits(QSerialPort::Data8);
        uno->setParity(QSerialPort::NoParity);
        uno->setStopBits(QSerialPort::OneStop);
        uno->setFlowControl(QSerialPort::NoFlowControl);
    }
    else{
        // give error message if not available
        qDebug() << QString("Port error: Couldn't find the Arduino");
    }
}


void LightManager::mirrorScreen(QImage* img){
    QDataStream writeStream(uno);
    int ledWidth = img->width() / numLeds;
    QVector<int> redAvg(numLeds, 0);
    QVector<int> greenAvg(numLeds, 0);
    QVector<int> blueAvg(numLeds, 0);
    QVector<int> n(numLeds, 0);


    //get sum of squares of RBG values
    for(int y = 0; y < img->height(); y++){
        QRgb* line = reinterpret_cast<QRgb*>(img->scanLine(y));
        int avgIndex = 0;
        for(int x = 0; x < img->width(); x += ledWidth){
            for(int i = 0; i < ledWidth && i+x < img->width(); i++){
                QRgb &rgb = line[x + i];
                redAvg[avgIndex] += qRed(rgb)^2;
                greenAvg[avgIndex] += qGreen(rgb)^2;
                blueAvg[avgIndex] += qBlue(rgb)^2;
                n[avgIndex]++;
            }
            if(avgIndex + 1 < numLeds)
                avgIndex++;
        }
    }
    //average out and write
    for(int i = 0; i < numLeds; i++){
        int tempRed = sqrt(redAvg[i] / n[i]);
        int tempGreen = sqrt(greenAvg[i] / n[i]);
        int tempBlue = sqrt(blueAvg[i] / n[i]);
        writeStream << (quint8)tempRed;
        writeStream << (quint8)tempGreen;
        writeStream << (quint8)tempBlue;
    }
}
void LightManager::mirrorGhosts(int dist){
    QDataStream writeStream(uno);
    writeStream << (quint8)(255);
    if(dist == 1000){ //default distance, should not be this when actually in game
        for(int i = 0; i < numLeds; i++){
            writeStream << (quint8)0;
            writeStream << (quint8)0;
            writeStream << (quint8)100;
        }

    }
    if(dist > 20){
        for(int i = 0; i < numLeds; i++){
            writeStream << (quint8)(255-dist);
            writeStream << (quint8)0;
            writeStream << (quint8)0;
        }
    }
    else{

        for(int i = 0; i < numLeds; i++){
            if(alternating){
                if(i % 2 == 0){
                    writeStream << (quint8)(255-dist);
                }
                else{
                    writeStream << (quint8)0;
                }

                writeStream << (quint8)0;
                writeStream << (quint8)0;
            }
            else{
                if((i + 1) % 2 == 0){
                    writeStream << (quint8)(255-dist);
                }
                else{
                    writeStream << (quint8)0;
                }

                writeStream << (quint8)0;
                writeStream << (quint8)0;
            }
        }
        alternating = !alternating;
    }
}
