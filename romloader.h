#ifndef ROMLOADER_H
#define ROMLOADER_H

#include <QFile>
#include <QString>
#include <QDebug>
#include "defs.h"


class RomLoader
{
public:
    QByteArray nesTestRom;
    QByteArray donkeyKongRom;
    u8* donkeyKongBytes;
    RomLoader();
};

#endif // ROMLOADER_H
