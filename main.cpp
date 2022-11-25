#include "window.h"
#include "weather.h"
#include "romloader.h"

#include <QApplication>
#include <QCommandLineParser>
#include <QDebug>
#include <QTime>
#include <QFuture>
#include <QtConcurrent>
#include "nesemulator.h"

#define MATT_TEST 0

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
    RomLoader* romLoader = new RomLoader();


    auto& bus = NESEmulator::Bus::the();
    auto& cpu = NESEmulator::CPU::the();
    auto& ppu = NESEmulator::PPU::the();

    NESEmulator::powerOn(*romLoader);
    // TODO (Jon): Delete?


    // Setup Pixels for game
    int numOfPixels = 240 * 256;
    u32* pixels = new u32[numOfPixels];
    for (int i = 0; i < numOfPixels; i++) {
        pixels[i] = 0;
    }
    bool running = true;
    Window w(pixels);
    w.show();
    ppu.initialize(pixels, w.glWidget());

    //ppu.debug_drawToScreen(romLoader->debug_getDonkeyKongRom());

#if MATT_TEST
    RomLoader loader;
    auto& bus = NESEmulator::Bus::the();

    bus.mattCPUTestLoadROM(loader.nesROM());
    
    cpu.execLoop();
#endif

    bus.execLoop();

    auto func = [&] () {
	while(running) {
	    bus.execLoop();
	    w.update();
	    //a.processEvents();
	}
    };
    QFuture<void> future = QtConcurrent::run(func);
    
    //return 0;
    return a.exec();
}
