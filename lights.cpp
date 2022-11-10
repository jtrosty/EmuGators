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
        uno->setBaudRate(QSerialPort::Baud9600);
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


void LightManager::setMode(int mode){
    if(uno->isWritable()){
        switch(mode){
            case 0:
                uno->write(QString('0').toStdString().c_str());
                break;
            case 1:
                uno->write(QString('1').toStdString().c_str());
                break;
            case 2:
                uno->write(QString('2').toStdString().c_str());
                break;


        }
    }
    else{
        qDebug() << "Couldn't write to serial!";
    }
}
