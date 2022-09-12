#ifndef BUS_H
#define BUS_H

#include "defs.h"
#include <QByteArray>
#include <QDebug>

class Bus
{
public:
    /* Memory
     *RAM
     *0x0000 to 0x1FFF
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
     */
    u8* memory; // Size 0xFFFF
    u16 ramStart = 			0x0000;
    u16 ramMirror = 		0x1FFF;
    u16 ppuIOStart = 		0x2000;
    u16 apuControlIOStart = 0x4000;
    u16 cartridgeROM = 		0x0800;

    Bus();
    ~Bus();

    void loadROM(QByteArray rom);

    u8 cpuReadMemory(u16 addr);
    void cpuWriteMemory(u16 addr, u8 data);

    /*
     *
     * PPU stuff will go here.
     *
     */
};

#endif // BUS_H
