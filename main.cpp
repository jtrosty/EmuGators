#include "ppu.h"
#include "window.h"
#include "weather.h"

#include <QApplication>
#include <QCommandLineParser>
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
    
    auto& cpu = CPU::the();
    cpu.execLoop();
    auto& ppu = NESEmulator::PPU();



    
    Window w;
    w.show();
    return a.exec();
}
