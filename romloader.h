#ifndef ROMLOADER_H
#define ROMLOADER_H

#include <QFile>
#include <QString>
#include <QDebug>
#include "defs.h"


class RomLoader
{
    QByteArray nesTestRom;
    QByteArray donkeyKongRom;
    QByteArray rom;
    u8* donkeyKongBytes;
public:
    
    QByteArray& nesROM() { return nesTestRom; }
    QByteArray const& nesROM() const { return nesTestRom; }
    QByteArray const& getRom() const { return rom; }
    QByteArray& debug_getDonkeyKongRom();
    void loadRom(QString path);
    RomLoader();
};

#endif // ROMLOADER_H
