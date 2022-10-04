#ifndef PPU_H
#define PPU_H

#include <QByteArray>

#include "defs.h"
#include "bus.h"
#include "romloader.h"

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
     * 8 kb total, 4kb section.
     * 16 by 16 grid of sprite tiles.
     * each grid of sprites is 8 by 8 pixels.
     * They are 8 by 8 bitmap. Each tile is stored in 2 bit planes.
     * the LSB and MSB.
     * In memory the LSB is stored then the most significant bit.
     * 16 bytes to store each tile.
     *
     * 0x2000- 0x23FF - NameTable 0
     * 0x2400- 0x27FF - NameTable 1
     * 0x2800- 0x2BFF - NameTable 2
     * 0x2C00- 0x2FFF - NameTable 3
     *
     * 0x3F00 - 3F10 - Pallate table
     * 0x3F00 is typically the background. 8 bit value and indexes
     * a specific color. Each
     * The first 4 palettes are for background
     * The last 4 are for foreground.
     */

    u16 spriteStart = 0x0000;
    u16 patternTable0 = 0x0000;
    u16 patternTable1 = 0x1000;
    u16 nameTableStart = 0x2000;
    u16 paletteMemStart = 0x3F00;

public:
    void initialize(QByteArray* glPixelData);
    ~PPU();
    void debug_load_vRam();
    void renderNameTable();
    void debug_drawPatternTable(int patternTable);
    void debug_setPixelPatternTable(int patternTable, u16 x, u16 y, u32 colorValue);
    void debug_loadRomDisplayVram();

private:
    void ppuWriteRegister(u16 address, u8 data);
    u8 ppuReadRegister(u16 address);
    void ppuWriteVRAM(u16 address, u8 data);
    u8 ppuReadVRAM(u16 address);
    u8 getPalette(u16 address);
    u32 debug_patternTable[2][128 *128]; // Each pattern table is 128 pixel wide and tall.
    void debug_patternTableToPixels(int patternTable, int x, int y);
    void debug_loadVRam();

};

}

using NESEmulator::PPU;
#endif // PPU_H
