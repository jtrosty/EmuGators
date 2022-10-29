#include "ppu.h"

namespace NESEmulator {

    PPU::PPU() {

    }

    void PPU::initialize(u32* glPixelArray) {
        pixelData = glPixelArray; }

    PPU::~PPU() {

    }


    void PPU::ppuWriteRegister(u16 address, u8 data) {
        Bus::the().writeMemory(address, data);
        // control register

        // mask register


       // status register


        // scrooll register

        // addr

        // data
    }

    u8 PPU::ppuReadRegister(u16 address) {
        u16 index = address & 0x0007;
        u8 result = 0;

        switch (index) {
        case 0x0000: {
            result = ppuControl.reg;
        }
        case 0x0001: {
            result = ppuMask.reg;
        }
        case 0x0002: {
            result = ppuStatus.reg * 0xE0;

            ppuStatus.verticalBlank = 0;
            // TODO(Jon): Clear latch by PPUSCROL and PPUADDR.
        }
        case 0x0003: {
            result = ppuControl.reg;
        }
        case 0x0004: {
            result = ppuControl.reg;
        }
        case 0x0005: {
            result = ppuControl.reg;
        }
        case 0x0006: {
            result = ppuControl.reg;
        }
        case 0x0007: {
            result = ppuControl.reg;
        }
        case 0x0008: {
            result = ppuControl.reg;
        }
        default: {

        }
        }

        return Bus::the().readMemory(address);
    }

    void PPU::ppuWriteVRAM(u16 address, u8 data) {
        vRam[address] = data;
    }

    u8 PPU::ppuReadVRAM(u16 address) {
        return vRam[address];
    }

    u8 PPU::getPalette(u16 address) {
        u8 result = 0x00;
        return result;
    }

    void PPU::debug_drawToScreen(QByteArray donkeyKong) {
        int prgRomSize = 0x01 * 0x4000; // 16 kb time number of prg rom secions
        int chrRomSize = 0x01 * 0x2000; // 8 kb time number of prg rom secions
        int prgRomStart = 0x10;// + 0x200; // 512
        int chrRomStart = prgRomStart + prgRomSize;
        int sizeArray = donkeyKong.size();
        // Load pattern table
        for (int i = 0; i < chrRomSize; i++) {
            vRam[i] = donkeyKong[chrRomStart + i];
        }
        debug_drawPatternTable(0);
        debug_patternTableToPixels(0, 0, 0);
        //debug_drawPatternTable(1);
        //debug_patternTableToPixels(1, 0, 0);
    }

    void PPU::debug_patternTableToPixels(int patternTable, int x, int y) {
        //u8* bytePatternTable = debug_patternTable[patternTable][0];
        u32 testarray[256 * 240];
        u32 testarray2[256 * 240];
        /*
        for (int i = 0; i < (256 * 240); i++) {
            testarray[i] = pixelData[i];
        }
        int offset = (x * 256) + y;
        for (int row = 0; row < 128; row++) {
            for (int col = 0; col < 128; col++) {
                Q_ASSERT((offset + (row * 256) + col) < (256 * 240));
                pixelData[(offset + (row * 256)) + (col)] = debug_patternTable[patternTable][(row * 128) + col];
            }
        }
        for (int i = 0; i < (256 * 240); i++) {
            testarray2[i] = pixelData[i];
        }

        */
        for (int i = 0; i < (128 * 128); i++) {
            //pixelData[i] = colors[6];
            pixelData[i] = debug_patternTable[patternTable][i];
        }
    }

    void PPU::debug_drawPatternTable(int patternTable) {
        // This loops for each of the

        u16 spritesInTable = 16;
        u16 pixelInTable = 8;
        int totalNumberOfSprites = spritesInTable * spritesInTable;
        int bitsInSprite = 64;
        u8 patternLSB;
        u8 patternMSB;
        u16 offset;
        u16 sprite0Start = 0x1000;
        /*
        if (patternTable == 0) 	offset = patternTable0;
        else 					offset = patternTable1;
        */

        // Loop through each sprite table in pattern table, there is 16x16 in each
        for (u16 spriteY = 0; spriteY < spritesInTable; spriteY++) {
            for (u16 spriteX = 0; spriteX < spritesInTable; spriteX++) {
                offset = spriteY * 256 + spriteX * 16;

                 //Loop through each pixel in each sprite, 8x8
                for (u16 row = 0; row < pixelInTable; row++) {

                    // Get the LSB and MSB from the bit plane
                    // there is a 1 byte offset from LSB byte and MST byte
                    patternLSB = ppuReadVRAM(patternTable * sprite0Start + offset + row + 0x0000);
                    patternMSB = ppuReadVRAM(patternTable * sprite0Start + offset + row + 0x0008);

                    for (u16 col = 0; col < pixelInTable; col++) {
                        // Need to loop through each byute plane and generate the value for each pixel
                        // The value determines the color from the palette table
                        // ONly need 1 bit from each of the MSB an LSB
                        u8 pixelColorValue = (patternLSB & 0x01) + ((patternMSB & 0x01 )<< 1 );
                        int x = (spriteX * 8 + (7 - col));
                        int y = (spriteY * 8 + row);
                        switch(pixelColorValue) {
                        case 0:
                            pixelColorValue = 0; // Black
                            break;
                        case 1:
                            pixelColorValue = 11; // blue
                            break;
                        case 2:
                            pixelColorValue = 34; // yellow
                            break;
                        case 3:
                            pixelColorValue = 40; // green
                            break;
                        default:
                            pixelColorValue = 6; // light blue
                            break;
                        }

                        u32 color32Byte = colors[pixelColorValue];
                        // NOTE this does not use the actual palette table #######################
                        debug_setPixelPatternTable(patternTable, x, y, color32Byte);
                        // move over a bit for the next pixel
                        patternLSB = patternLSB >> 1;
                        patternMSB = patternMSB >> 1;
                    }
                }
            }
        }
    }

    void PPU::debug_setPixelPatternTable(int patternTable, u16 x, u16 y, u32 colorValue) {
        int index = (y * 128) + x;
        Q_ASSERT(index < (128 * 128));
        debug_patternTable[patternTable][(y * 128) + x] = colorValue;
    }

}

