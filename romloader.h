#ifndef ROMLOADER_H
#define ROMLOADER_H

#include <QFile>
#include <QString>
#include <QDebug>


class RomLoader
{
    QByteArray nesTestRom;
public:
    
    QByteArray& nesROM() { return nesTestRom; }
    QByteArray const& nesROM() const { return nesTestRom; }

    RomLoader();
};

#endif // ROMLOADER_H
