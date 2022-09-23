#ifndef PPU_H
#define PPU_H

#include <QByteArray>

#include "defs.h"
#include "bus.h"

namespace NESEmulator {

class PPU : public Device<PPU>
{
private:
    const u32 colors[55] = {
        0x000000ff, 0xfcfcfcff, 0xf8f8f8ff, 0xbcbcbcff, 0x7c7c7cff, 0xa4e4fcff,
        0x3cbcfcff, 0x0078f8ff, 0x0000fcff, 0xb8b8f8ff, 0x6888fcff, 0x0058f8ff,
        0x0000bcff, 0xd8b8f8ff, 0x9878f8ff, 0x6844fcff, 0x4428bcff, 0xf8b8f8ff,
        0xf878f8ff, 0xd800ccff, 0x940084ff, 0xf8a4c0ff, 0xf85898ff, 0xe40058ff,
        0xa80020ff, 0xf0d0b0ff, 0xf87858ff, 0xf83800ff, 0xa81000ff, 0xfce0a8ff,
        0xfca044ff, 0xe45c10ff, 0x881400ff, 0xf8d878ff, 0xf8b800ff, 0xac7c00ff,
        0x503000ff, 0xd8f878ff, 0xb8f818ff, 0x00b800ff, 0x007800ff, 0xb8f8b8ff,
        0x58d854ff, 0x00a800ff, 0x006800ff, 0xb8f8d8ff, 0x58f898ff, 0x00a844ff,
        0x005800ff, 0x00fcfcff, 0x00e8d8ff, 0x008888ff, 0x004058ff, 0xf8d8f8ff,
        0x787878ff
    };
    QByteArray* pixelData;
    Bus* bus;

    u8 vRam[0x4000] {0};
    /*
     * 0x0000- 0x0FFF - Pattern Table 0
     * 0x1000- 0x1FFF - Pattern Table 1
     * 0x2000- 0x23FF - NameTable 0
     * 0x2400- 0x27FF - NameTable 1
     * 0x2800- 0x2BFF - NameTable 2
     * 0x2C00- 0x2FFF - NameTable 3
     *
     *
     *
     *
     *
     *
     */
    u16 spriteStart = 0x0000;
    u16 nameTableStart = 0x2000;
    u16 paletteMemStart = 0x3F00;

public:
    void initialize(QByteArray* glPixelData);
    ~PPU();
    void debug_load_vRam();
    void renderNameTable();
    void drawSprite(int x, int y);


private:
    void ppuWriteRegister(u16 address, u8 data);
    u8 ppuReadRegister(u16 address);
    void ppuWriteVRAM(u16 address, u8 data);
    u8 ppuReadVRAM(u16 address);

};

}

using NESEmulator::PPU;
#endif // PPU_H
