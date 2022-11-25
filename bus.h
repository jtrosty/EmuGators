#ifndef BUS_H
#define BUS_H

#include "badge.h"
#include "defs.h"
#include "device.h"
#include "forward.h"
#include <QByteArray>
#include <QDebug>
#include "romloader.h"

namespace NESEmulator {

class Bus : public Device<Bus>
{
    /* Memory
     *RAM
     * 0x0000 to 0x1FFF
     * Zero Page memory 0x0000 to 0x00FF
     * Stack memory 0x100-0x01FF
     * 0x0200 to 0x07FF -RAM
     * 0800 0x1FFF is just mirros of the first 0x000 to 0x07FF
     *
     *PPU IO Ports
     *0x2000 to 0x3FFF
     *
     *APU/Controller IO Ports
     *0x4000 to 0x5FFF
     *
     *8kb Cartridge RAM WRAM
     *0x6000 to 0x7FFF
     *
     *32kb Cartridge ROM
     *0x8000 to 0xEFFF
     *
     *NMI REset IRQ vectors?
     *0xFFFA to 0xFFFF
     * The Non-Maskable Interrupt or NMI ($FFFA),
     * the Reset Vector ($FFFC),
     * and the IRQ/BRK Vector ($FFFE).
     * The hex values that are in parenthesis are where the NES looks to
     * find the 16 bit location of where to jump to within the NES.
     */

    u8* memory; // Size 0xFFFF
    u16 mRamStart = 		0x0000;
    u16 ramMirror = 		0x1FFF;
    u16 ppuMirror = 		0x3FFF + 0x2000;
    u16 ppuIOStart = 		0x2000;
    u16 apuControlIOStart = 0x4000;
    u16 cartridgeROM = 		0x8000;
    u16 ppuRegisterStart =  0x2000;
    u16 ppuRegisterEnd =    0x3FFF;
    u8 mController[2] { 0 };
    u8 mControllerCache[2] { 0 };
    u16 pcStartAddress = 0xFFFC;
    u8 performDMA = false;
    u8 dataDMA = 0x00;

    u32 clockCycle = 0;
    u16 addrNMI = 0xFFFA;
public:
    ~Bus();
    void initialize(QByteArray romToLoad);
    void reset() { } // dummy for now
    void execLoop();

    ALWAYS_INLINE void updateController(Badge<GLWidget>, u8 controller) { *mController = controller; }
    ALWAYS_INLINE u8 getController(Badge<GLWidget>) { return *mController; }
    
    u8* rawMemory() { return memory; }
    u8 debug_prgRom[0x3FFF];
    
    u16 ramStart() const { return mRamStart; }
    u16 pcStartPoint() const { return pcStartAddress; }
    QByteArray romLoaded;
    
    void mattCPUTestLoadROM(QByteArray rom);
    void loadROM();
    void loadROM(QByteArray rom);

    u8 readMemory(u16 addr);
    u16 readMemory16Bits(u16 addr);
    void writeMemory(u16 addr, u8 data);
    void writeMemory16Bits(u16 addr, u16 data);

    /*
     * PPU stuff will go here.
     */
    u8 dmaAddr = 0x00;
    u8 perforDMA = false;
    u8 startTransferDMA = false;
    u8* pDMA;// = (u8*)memory[0x4014];
};

}
using NESEmulator::Bus;
#endif // BUS_H
