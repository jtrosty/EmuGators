#include "ppu.h"

namespace NESEmulator {

    PPU::PPU() {

    }

    void PPU::initialize(u32* glPixelArray) {
        pixelData = glPixelArray; }

    PPU::~PPU() {

    }


    void PPU::ppuWriteRegister(u16 address, u8 data) {
        u16 index = address & 0x0007;

        switch (index) {
        case 0x0000: {
            ppuControl.reg = data;
            break;
        }
        case 0x0001: {
            ppuMask.reg = data;
            break;
        }
        case 0x0002: {
            // No Write
            break;
        }
        case 0x0003: {
            //OAM - Object Attribute Memory address
            // Wirte this looksl ike it is usally set to zero.
            //TODO: Figure this out.
            ppuOAMAddr = 0;

            break;
        }
        case 0x0004: {
            // Can be read in rare instances
            // OAM data
            // DO NOT WRITEE DIRECTLY TO THIS REGISTER IN MOST CASES PER NESDEV
            break;
        }
        case 0x0005: {
            // ppuScrol Tells ppu which pixel of nametable selected through ppuctr
            // should be at the top left corner of rendered screen
            // Typically written during vertical blanking. Change made during rendering wont take
            // affect till the next grame.
            // Write only

            break;
        }
        case 0x0006: {
            // Write only
            // ppu addess, write only
            ppuADDR = data;
            break;
        }
        case 0x0007: {
            // VRAM read/write data register
            if (ppuADDR < 0x3F00) {

            }
            else if (ppuADDR >= 0x3F00) {
            }
            // Increment the nametable addressw

            // Depends on if in horizontal mode or veritcal mode.  Logic not yet yet
            // TODO: Need logic for if veritcal or horizontalmode
            if (true) {
                // If vertical mode
                ppuADDR += 32;
            }
            else {
                // if horizontal mode
                ppuADDR += 1;
            }

            break;
        }
        default: {
            printf("Accessed write ppu registers, but got default. Index value %i", index);
        }
        }
    }

    u8 PPU::ppuReadRegister(u16 address) {
        u16 index = address & 0x0007;
        u8 result = 0;

        switch (index) {
        case 0x0000: {
            result = ppuControl.reg;
            break;
        }
        case 0x0001: {
            result = ppuMask.reg;
            break;
        }
        case 0x0002: {
            result = ppuStatus.reg & 0xE0;

            ppuStatus.verticalBlank = 0;
            ppuAddressLatch = 0;
            // TODO(Jon): Clear latch by PPUSCROL and PPUADDR.
            break;
        }
        case 0x0003: {
            // Does not get read
            //OAM - Object Attribute Memory address
            break;
        }
        case 0x0004: {
            // Can be read in rare instances
            // OAM data
            break;
        }
        case 0x0005: {
            // ppuScrol
            // Write only
            break;
        }
        case 0x0006: {
            // Write only
            // ppu addess, write only
            break;
        }
        case 0x0007: {
            // VRAM read/write data register
            result = ppuDATA;
            if (ppuADDR < 0x3F00) {

            }
            else if (ppuADDR >= 0x3F00) {
                result = ppuDATA;
            }
            // Increment the nametable addressw

            // Depends on if in horizontal mode or veritcal mode.  Logic not yet yet
            // TODO: Need logic for if veritcal or horizontalmode
            if (true) {
                // If vertical mode
                ppuADDR += 32;
            }
            else {
                // if horizontal mode
                ppuADDR += 1;
            }

            break;
        }
        default: {
            printf("Accessed read ppu registers, but got default. Index value %i", index);
        }
        }
        return result;
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

    void PPU::executeLoop() {
        if (scanline >= -1 && scanline < 240) {

            // This cycle is skpped for scanline 0
            if (scanline == 0 && cycle == 0) {
                cycle = 1;
            }

            // Dummy scan line this is to fill the registers with data to being the visible scanlines
            if(scanline == -1 || scanline == 261) {
                if (scanline == -1 && cycle == 1) {
                    // start a new frame
                    ppuStatus.verticalBlank = 0;
                }

            }
            if (cycle >= 1 && cycle < 258) {

                // during visible seciton every 8 cycles updatessome variable.
                // It takes 2 cycles to write a 16-bit word.
                // other other case will be skipped.
                switch (cycle % 8) {
                // NT byte
                case 0:
                    ppuReadVRAM(nameTableStart | vram.reg & 0x0FFF);
                    break;
                case 1:
                    // Skip, it takes 2 cycles to perform the task in case 0
                    break;
                // AT Byte
                case 2:
                    // To get to the attribute, 12 bits total to get to the attribute
                    // from NESDEV.org:
                    // NN 1111 YYY XXX
                    // || |||| ||| +++-- high 3 bits of coarse X (x/4)
                    // || |||| +++------ high 3 bits of coarse Y (y/4)
                    // || ++++---------- attribute offset (960 bytes)
                    // ++--------------- nametable select
                    ppuReadVRAM(nameTableAttributeStart |
                                vram.nameTableY << 11 |
                                vram.nameTableX << 10 |
                                vram.coarseX >> 2 |
                                (vram.coarseY >> 2) << 3);
                    break;
                case 3:
                    // Skip, it takes 2 cycles to perform the task above
                    break;
                // Pattern Table LSB
                case 4:
                    break;
                case 5:
                    break;
                // Pattern Table MSB
                case 6:
                    break;
                case 7:
                    break;
                }
            }
        }
        if ( scanline == 240) {

        }
        if ( scanline >= 241 && scanline <= 260) {
            if (scanline == 241 && cycle ==1) {
                ppuStatus.verticalBlank = 1;
            }

        }
    }

    void PPU::debug_patternTableToPixels(int patternTable, int x, int y) {
        //u8* bytePatternTable = debug_patternTable[patternTable][0];
        u32 testarray[256 * 240];
        u32 testarray2[256 * 240];
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

