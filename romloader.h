#ifndef ROMLOADER_H
#define ROMLOADER_H

#include <QFile>
#include <QString>
#include <QDebug>


class RomLoader
{
public:
    QByteArray nesTestRom;
    QByteArray donkeyKongRom;
    RomLoader();
};

#endif // ROMLOADER_H
