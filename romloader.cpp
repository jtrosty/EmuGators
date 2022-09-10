#include "romloader.h"

RomLoader::RomLoader() {
    QString path = ":/resources/roms/";
    QString nestest = "nestest.nes";
    QFile file(path + nestest);
    if (!file.open(QIODevice::ReadOnly)) {
        qInfo("File faield to open");
    }
    else {
        nesTestRom = file.readAll();
    }
}
