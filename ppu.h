#ifndef PPU_H
#define PPU_H
#include "device.h"
#pragma once

#include <stdio.h>
#include <QByteArray>
#include <QDebug>

#include "defs.h"
#include "bus.h"

namespace NESEmulator {

class PPU : public Device<PPU> {
private:
    // The color palette
    //https://lospec.com/palette-list/nintendo-entertainment-system
    u16 const numOfColors = 64;
    const u32 colors[64] = {
        0xff545454, 0xff041e74, 0xff081090, 0xff300088, 0xff440064,
        0xff5c0030, 0xff540400, 0xff3c1800, 0xff202a00, 0xff083a00,
        0xff004000, 0xff003c00, 0xff00323c, 0xff000000, 0xff000000,
        0xff000000,
        0xff989698, 0xff084cc4, 0xff3032ec, 0xff5c1ee4, 0xff8814b0,
        0xffa01464, 0xff982220, 0xff783c00, 0xff545a00, 0xff287200,
        0xff087c00, 0xff007628, 0xff006678, 0xff000000, 0xff000000,
        0xff000000,
        0xffeceeec, 0xff4c9aec, 0xff787cec, 0xffb062ec, 0xffe454ec,
        0xffec58b4, 0xffec6a64, 0xffd48820, 0xffa0aa00, 0xff74c400,
        0xff4cd020, 0xff38cc6c, 0xff38b4cc, 0xff3c3c3c, 0xff000000,
        0xff000000,
        0xffeceeec, 0xffa8ccec, 0xffbcbcec, 0xffd4bcec, 0xffecaeec,
        0xffecaed4, 0xffecb4b0, 0xffe4c490, 0xffccd278, 0xffb4de78,
        0xffa8e290, 0xff98e2b4, 0xffa0d6e4, 0xffa0a2a0, 0xff000000,
        0xff000000
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
     * 0x3F00 - 3F1F - Pallate table
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
    u32 const pixelWidth = 256;
    u32 pixelHeight = 240;

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
            u8 slave : 1;
            u8 NMI : 1;
        };
        u8 reg = 0x0000;
    } ppuControl;

    union PPUMASK {
        struct {
            // The colon then 1 indicates that each of these only require 1 bit
            // The below is  bit table.
            u8 greyScale : 1;
            u8 backgroundLeft : 1;
            u8 spritesLeft : 1;
            u8 renderBackground : 1;
            u8 renderSprites : 1;
            u8 emphasizeRed : 1;
            u8 emphasizeGreen : 1;
            u8 emphasizeBlue : 1;
        };
        u8 reg = 0x0000;
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
        u8 reg = 0x0000;
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
        u16 reg = 0x0000;
    };
    u8 fineX = 0x00;
    LoopyReg vramLoopy;
    LoopyReg tempVramLoopy;

    // TODO (Jon) remove unused variables
    //u8 ppuOAMAddr;
    //u8 ppuSCROLL; // maybe
    u8 ppuDATA = 0x00; // Maybe not needed
    u8 ppuAddressLatch = 0x00;

    int scanline = 0;
    u16 cycle = 0;
    u8 mirroring = 2; // 2 for it didn't get set, 1 for veritcal, 0 for horizontal

    // Background variables
    // These are loaded in preparation of the next 8 cycles.
    u8 bgNextNametableValue = 0x00;
    u8 bgNextTileAttribute = 0x00;
    u8 bgPatternLSB = 0x00;
    u8 bgPatternMSB = 0x00;

    // Shifters these are the current shifters
    // These are used during the current 8 cycles
    u16 patternTableShifterHi = 0x0000;
    u16 patternTableShifterLow = 0x0000;
    u16 palleteShifterHi = 0x0000;
    u16 palleteShifterLow = 0x0000;

    void setCurrentShifter();
    // Helpers for calculating what 2 bits of attribute table byte are needed
    u8 topOrBottom = 0x00;
    u8 leftOrRight = 0x00;

    u8 NMIFlag = 0;



    struct ObjectAttributeMemory {
        u8 yPosition;
        u8 idPattern;
        u8 attribute;
        u8 xPosition;
    } OAM[64] {0};



public:
    PPU();
    ~PPU();
    u8 getNMI() {return NMIFlag;};
    void setNMItoZero() {NMIFlag = 0;};
    u8 getMirorring() { return mirroring; };
    void setMirroring(u8 value) {mirroring = value; };
    void renderNameTable();
    void initialize(u32* glPixelData);
    void executeLoop();
    void loadVram(QByteArray &rom, u8 num8kVram, u16 chrRomStart);

    void debug_load_vRam();
    void debug_drawPatternTable(int patternTable);
    void debug_setPixelPatternTable(int patternTable, u16 x, u16 y, u32 colorValue);
    void debug_loadRomDisplayVram();
    void debug_drawToScreen(QByteArray donkeyKong);
    void ppuWriteRegister(u16 address, u8 data);
    u8 ppuReadRegister(u16 address);

    // OAM and Sprite rendering
    u8 oamAddress = 0x00;
    u8* pOAM = (u8*)OAM;


private:
    void drawSprites();
    void writeToBus(u16 address, u8 data);
    u8 readFromBus(u16 address);
    void ppuWriteVRAM(u16 address, u8 data);
    u8 ppuReadVRAM(u16 address);
    u8 getPalette(u16 address);
    void incrementX();
    void incrementY();
    void loopyTransferX();
    void loopyTransferY();
    void setPixel(u32 x, u32 y, u32 color);

    u32 debug_patternTable[2][128 * 128]; // Each pattern table is 128 pixel wide and tall.
    void debug_patternTableToPixels(int patternTable, int x, int y);

};

}

using NESEmulator::PPU;
#endif // PPU_H
