#ifndef PPU_H
#define PPU_H
#pragma once

#include <stdio.h>
#include <QByteArray>

#include "defs.h"
#include "bus.h"

namespace NESEmulator {

class PPU : public Device<PPU>
{
private:
    // The color palette
    //https://lospec.com/palette-list/nintendo-entertainment-system
    u16 const numOfColors = 55;
    const u32 colors[55] = {
        0xff000000, 0xfffcfcfc, 0xfff8f8f8, 0xffbcbcbc, 0xff7c7c7c, 0xffa4e4fc,
        0xff3cbcfc, 0xff0078f8, 0xff0000fc, 0xffb8b8f8, 0xff6888fc, 0xff0058f8,
        0xff0000bc, 0xffd8b8f8, 0xff9878f8, 0xff6844fc, 0xff4428bc, 0xfff8b8f8,
        0xfff878f8, 0xffd800cc, 0xff940084, 0xfff8a4c0, 0xfff85898, 0xffe40058,
        0xffa80020, 0xfff0d0b0, 0xfff87858, 0xfff83800, 0xffa81000, 0xfffce0a8,
        0xfffca044, 0xffe45c10, 0xff881400, 0xfff8d878, 0xfff8b800, 0xffac7c00,
        0xff503000, 0xffd8f878, 0xffb8f818, 0xff00b800, 0xff007800, 0xffb8f8b8,
        0xff58d854, 0xff00a800, 0xff006800, 0xffb8f8d8, 0xff58f898, 0xff00a844,
        0xff005800, 0xff00fcfc, 0xff00e8d8, 0xff008888, 0xff004058, 0xfff8d8f8,
        0xff787878
    };
    u32* pixelData;
    //NESEmulator::Bus* bus;

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

    u16 ramPPUStart = 0x2000;
    u16 ramPPUEnd = 0x3FFF;
    u16 spriteStart = 0x0000;
    u16 patternTable0 = 0x0000;
    u16 patternTable1 = 0x1000;
    u16 nameTableStart = 0x2000;
    u16 nameTableAttributeStart = 0x23C0;
    u16 paletteMemStart = 0x3F00;
    u16 pixelWidth = 256;
    u16 pixelHeight = 240;

    // PPU registers

    union PPUCTRL {
        struct {
            // The colon then 1 indicates that each of these only require 1 bit
            // The below is  bit table.
            u8 scrollingNametableX : 1;
            u8 scrollingNametableY : 1;
            u8 VramAddressIncrement : 1;
            u8 spritePatternTable : 1;
            u8 backgroundPatternTable : 1;
            u8 spriteSize : 1;
            u8 NMI : 1;
            u8 slave : 1;
        };
        u8 reg;
    } ppuControl;

    union PPUMASK {
        struct {
            // The colon then 1 indicates that each of these only require 1 bit
            // The below is  bit table.
            u8 greyScale : 1;
            u8 backgroundLeft : 1;
            u8 backgroundRight : 1;
            u8 renderBackground : 1;
            u8 renderSprites : 1;
            u8 emphasizeRed : 1;
            u8 emphasizeGreen : 1;
            u8 emphasizeBlue : 1;
        };
        u8 reg;
    } ppuMask;

    union PPUSTATUS {
        struct {
            // The colon then 1 indicates that each of these only require 1 bit
            // The below is  bit table.
            u8 ppuOpenBus : 5;
            u8 spriteOverflow : 1;
            u8 spriteZeroHit : 1;
            u8 verticalBlank : 1;
        };
        u8 reg;
    } ppuStatus;

    union LoopyReg {
        struct {
            u16 coarseX : 5;
            u16 coarseY : 5;
            u16 nameTableX : 1;
            u16 nameTableY : 1;
            u16 fineY : 3;
            u16 unused : 1;
        };
        u16 reg;
    };
    u8 fineX = 0x00;
    LoopyReg vram;
    LoopyReg tempVram;

    // TODO (Jon) remove unused variables
    u8 ppuOAMAddr;
    u8 ppuSCROLL; // maybe
    u16 ppuADDR;
    u8 ppuDATA; // Maybe not needed
    u8 ppuAddressLatch;

    u16 scanline = 0;
    u16 cycle = 0;

    // Background variables
    u8 bgNametableValue = 0;
    u8 bgTileAttribute = 0;
    u8 bgPatternLSB = 0;
    u8 bgPatternMSB = 0;


    struct ObjectAttributeMemory {
        u8 yPosition;
        u8 indexNumber;
        u8 attribute;
        u8 xPosition;
    } OAM[64];


public:
    PPU();
    ~PPU();
    void renderNameTable();
    void initialize(u32* glPixelData);
    void executeLoop();
    void loadVram(QByteArray rom, u8 num8kVram, u16 chrRomStart);

    void debug_load_vRam();
    void debug_drawPatternTable(int patternTable);
    void debug_setPixelPatternTable(int patternTable, u16 x, u16 y, u32 colorValue);
    void debug_loadRomDisplayVram();
    void debug_drawToScreen(QByteArray donkeyKong);
    void ppuWriteRegister(u16 address, u8 data);
    u8 ppuReadRegister(u16 address);

private:
    void writeToBus(u16 address, u8 data);
    u8 readFromBus(u16 address);
    void ppuWriteVRAM(u16 address, u8 data);
    u8 ppuReadVRAM(u16 address);
    u8 getPalette(u16 address);
    void incrementX();
    void incrementY();
    void loopyTransferX();
    void loopyTransferY();
    void setPixel(int x, int y, u32 color);

    u32 debug_patternTable[2][128 * 128]; // Each pattern table is 128 pixel wide and tall.
    void debug_patternTableToPixels(int patternTable, int x, int y);

};

}

using NESEmulator::PPU;
#endif // PPU_H
