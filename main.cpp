#include "ppu.h"
#include "window.h"
#include "weather.h"
#include "romloader.h"

#include <QApplication>
#include <QCommandLineParser>
#include <QDebug>
#include "nesemulator.h"

#define MATT_CPU_TEST 0

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QApplication::setApplicationName("NES Emulator");
    QApplication::setApplicationVersion("!Pre-alpha, if that!");
    
    QCommandLineParser parser
;
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
    //cpu.execLoop();

    // Setup Pixels for game
    int numOfPixels = 128 * 128;
    u32* pixels = new u32[numOfPixels];
    for (int i = 0; i < numOfPixels; i++) {
        pixels[i] = 0;
    }
    ppu.initialize(pixels);

    ppu.debug_drawToScreen(romLoader->debug_getDonkeyKongRom());

#if MATT_CPU_TEST
    RomLoader loader;
    auto& bus = NESEmulator::Bus::the();

    bus.mattCPUTestLoadROM(loader.nesROM());
    
    auto& cpu = NESEmulator::CPU::the();
    cpu.execLoop();
#endif
    
    Window w(pixels);
    w.show();
    return a.exec();
}
