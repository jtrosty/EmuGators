#include "romloader.h"

RomLoader::RomLoader() {
    QString path = ":/resources/roms/";
    QString nestest = "nestest.nes";
    QString donkeyKong = "donkey kong.nes";

    QFile file(path + nestest);
    if (!file.open(QIODevice::ReadOnly)) {
        qInfo("File faield to open");
    }
    else {
        nesTestRom = file.readAll();
    }
    file.close();

    QFile fileDonkey(path + donkeyKong);
    if (!fileDonkey.open(QIODevice::ReadOnly)) {
        qInfo("File faield to open");
    }
    else {
        donkeyKongRom = fileDonkey.readAll();
    }
    fileDonkey.close();
}
