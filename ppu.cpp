#include "ppu.h"

namespace NESEmulator {

    void PPU::initialize(QByteArray* glPixelArray) {
        pixelData = glPixelArray;
    }

    PPU::~PPU() {

    }


    void PPU::ppuWriteRegister(u16 address, u8 data) {
        Bus::the().writeMemory(address, data);
    }

    u8 PPU::ppuReadRegister(u16 address) {
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

    void PPU::debug_loadVRam() {


    }

    void PPU::debug_patternTableToPixels(int patternTable, int x, int y) {
        u8* bytePatternTable = (u8*)debug_patternTable[patternTable][0];
        int offset = x * 4 * 256 + (y * 4);
        for (int row = 0; row < 128; row++) {
            for (int col = 0; col < 128; col++) {
                //pixelData[(offset + (row * 256 * 4)) + (col * 4)] = byte[patternTable][(row * 128) + col];
            }
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
        if (patternTable == 0) 	offset = patternTable0;
        else 					offset = patternTable1;

        // Loop through each sprite table in pattern table, there is 16x16 in each
        for (u16 spriteX = 0; spriteX < spritesInTable; spriteX++) {
            for (u16 spriteY = 0; spriteY < spritesInTable; spriteY++) {
                //Loop through each pixel in each sprite, 8x8
                for (u16 row = 0; row < pixelInTable; row++) {
                    // Get the LSB and MSB from the bit plane
                    patternLSB = ppuReadVRAM(offset + (spriteX * spritesInTable) + spriteY + row + 0);
                    // there is a 1 byte offset from LSB byte and MST byte
                    patternMSB = ppuReadVRAM(offset + (spriteX * spritesInTable) + spriteY + row + 1);
                    for (u16 col = 0; col < pixelInTable; col++) {
                        // Need to loop through each byute plane and generate the value for each pixel
                        // The value determines the color from the palette table
                        // ONly need 1 bit from each of the MSB an LSB
                        u8 pixelColorValue = (patternLSB & 0x01) | (patternMSB & 0x01 << 1 );
                        int x = (spriteX * 8 + row);
                        int y = (spriteY * 8 + col);
                        u32 color32Byte = colors[pixelColorValue];
                        // NOTE this does not use the actual palette table #######################
                        debug_setPixelPatternTable(patternTable, x, y, color32Byte);
                        // move over a bit for the next pixel
                        patternLSB = patternLSB >> 1;
                        patternLSB = patternMSB >> 1;
                    }

                }
            }
        }
    }

    void PPU::debug_setPixelPatternTable(int patternTable, u16 x, u16 y, u32 colorValue) {
        debug_patternTable[patternTable][(x * 128) + y] = colorValue;
    }

    void PPU::debug_loadRomDisplayVram() {
        RomLoader debug_loadDonkeyKong;
        debug_loadDonkeyKong.donkeyKongRom;
    }



}

