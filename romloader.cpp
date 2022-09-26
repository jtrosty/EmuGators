#include "romloader.h"

RomLoader::RomLoader() {
    QString path = ":/resources/roms/";
    QString nestest = "nestest.nes";
    QString donkeyKong = "DonkeykongClassic.nes";
    QFile file(path + donkeyKong);
    if (!file.open(QIODevice::ReadOnly)) {
        qInfo("File faield to open");
    }
    else {
        nesTestRom = file.readAll();
    }
}
