#include "ppu.h"
#include "window.h"
#include "weather.h"
#include "romloader.h"

#include <QApplication>
#include <QCommandLineParser>
#include <QDebug>
#include "nesemulator.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QApplication::setApplicationName("NES Emulator");
    QApplication::setApplicationVersion("!Pre-alpha, if that!");
    
    QCommandLineParser parser;
    parser.setApplicationDescription("NES Emulator");
    parser.addHelpOption();
    parser.addVersionOption();

    parser.addPositionalArgument("ROM", QApplication::translate("main", "ROM to load into the emulator"));

    parser.process(a);
    
    const QStringList args = parser.positionalArguments();

    NESEmulator::powerOn();
    RomLoader* romLoader = new RomLoader();

    auto& cpu = CPU::the();
    auto& ppu = PPU::the();
    cpu.execLoop();
    int testSize = romLoader->nesTestRom.size();
    qInfo() << "The size of the rom is " << testSize << "\n";

    ppu.debug_drawToScreen(romLoader->donkeyKongRom);
    u32* pixels;

    Window w(pixels);
    w.show();
    return a.exec();
}
