#include "window.h"
#include "weather.h"

#include <QApplication>
#include <QCommandLineParser>
#include "nesemulator.h"

#define MATT_CPU_TEST 0

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

#if MATT_CPU_TEST
    RomLoader loader;
    auto& bus = NESEmulator::Bus::the();

    bus.mattCPUTestLoadROM(loader.nesROM());
    
    auto& cpu = NESEmulator::CPU::the();
    cpu.execLoop();
#endif
    
    Window w;
    w.show();
    return a.exec();
}
