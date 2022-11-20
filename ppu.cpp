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
            tempVram.nameTableX = ppuControl.scrollingNametableX;
            tempVram.nameTableY = ppuControl.scrollingNametableY;
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
            ppuWriteVRAM(vram.reg, data);
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

    void PPU::writeToBus(u16 address, u8 data)	{
        Bus::the().writeMemory(address, data);
    }

    u8 PPU::readFromBus(u16 address) {
        return Bus::the().readMemory(address);
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

    void PPU::loadVram(QByteArray rom, u8 num8kVram, u16 chrRomStart) {
        u32 chrRomSize = num8kVram * 0x2000;
        for (int i = 0; i < chrRomSize; i++) {
            vRam[i] = rom[chrRomStart + i];
        }

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

                // during visible seciton every 8 cycles updatessome variable to be used on the next cycle.
                // 1 cycle per pixel
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
                    bgTileAttribute = ppuReadVRAM(nameTableAttributeStart |
                                vram.nameTableY << 11 					  |
                                vram.nameTableX << 10   				  |
                                vram.coarseX >> 2 						  |
                                (vram.coarseY >> 2) << 3);
                    // TODO (Jon): I think there is more here

                    break;
                case 3:
                    // Skip, it takes 2 cycles to perform the task above
                    break;
                // Pattern Table LSB
                case 4:
                    //
                    // 0000 0000 0000 0000
                    //   X---------------- Pattern table selector
                    //      XXXX XXXX----- This is the nametable value,This byte selects 1 of 256 different patters on the table
                    //           XXXX XXXX the fine control called fine_y
                    bgPatternLSB = ppuReadVRAM((ppuControl.backgroundPatternTable << 12)
                                               + ((u16)bgNametableValue << 4)
                                               + (vram.fineY) + 0x0000);

                    break;
                case 5:
                    // Skip, it takes 2 cycles to perform the task above
                    break;
                // Pattern Table MSB
                case 6:
                    bgPatternMSB = ppuReadVRAM((ppuControl.backgroundPatternTable << 12)
                                               + ((u16)bgNametableValue << 4)
                                               + (vram.fineY) + 0x0008);
                    break;
                case 7:
                    // Skip, it takes 2 cycles to perform the task above
                    incrementX();
                    break;
                }
                if (cycle == 256) {
                    // Got to next scanline
                    incrementY();
                }
                if (cycle == 257) {
                   //
                    loopyTransferX();
                }
                if (cycle == 338 || cycle == 340) {
                    // The case 1-8 repeats from above after 280, but no more pixels are displayed to
                    // The screen, this below does nothign
                    ppuReadVRAM(nameTableStart | vram.reg & 0x0FFF);
                }
                if (scanline == -1 && cycle >= 280) {
                    // We have completed the row, in preperation for the next, perform
                    // Loopy register y transform from tvram to vram.
                    loopyTransferY();
                }
            }
        }
        if ( scanline == 240) {
            // Nothing happens, rendering is complete
        }
        if ( scanline >= 241 && scanline <= 260) {
            if (scanline == 241 && cycle ==1) {
                // The frame is done.
                ppuStatus.verticalBlank = 1;
                // tell CPU tyhat rendering is complete
                if (ppuControl.NMI) {
                    // Set NMI in RAM to true
                    Bus::the().writeMemory(0xFFFA, 1);
                }
            }
        }
        if (ppuMask.renderBackground) {
            // Perform redner of background,
            // Only 1 bit is needed that correlates with 0-7 based on wehre int eh cycle we are.
            u8 mask = 0x80 >> fineX;https://docs.google.com/document/d/1o9N4FHd5cBQrIEk_3XJsgFpvgkrLlJYZ4u9N1GTJgcc/edit
            // Shift bits 1, 1 bit per pixel
            bgPatternLSB <<= 1;
            bgPatternMSB <<= 1;

            u8 lowerBit = bgPatternLSB & mask;
            u8 higherBit = bgPatternMSB & mask;
            // Only need to knwo if the value is 1 or 0.
            if (lowerBit > 0) lowerBit = 0x01;
            if (higherBit > 0) higherBit = 0x01;
            // this pixel value gives you 0-4 and sets the color in the pallete
            u8 pixelColorValue = (lowerBit & 0x01) + ((higherBit & 0x01) << 1);
            // Now get the pallete that will be used
            u8 palleteLowBit = bgTileAttribute & mask;
            u8 palleteHighBit = bgTileAttribute & mask;
            u8 palleteValue = (palleteLowBit & 0x01) + ((palleteHighBit & 0x01) << 1);

            u32 pixelColor = colors[(ppuReadVRAM(paletteMemStart + pixelColorValue)) % numOfColors ];


            int x = 0;
            int y = 0;
            setPixel(cycle, scanline, pixelColor);

            /*
                    patternLSB = ppuReadVRAM(patternTable * sprite0Start + offset + row + 0x0000);
                    patternMSB = ppuReadVRAM(patternTable * sprite0Start + offset + row + 0x0008);

                    for (u16 col = 0; col < pixelInTable; col++) {
                        // Need to loop through each byute plane and generate the value for each pixel
                        // The value determines the color from the palette table
                        // ONly need 1 bit from each of the MSB an LSB
                        u8 pixelColorValue = (patternLSB & 0x01) + ((patternMSB & 0x01 )<< 1 );
                        int x = (spriteX * 8 + (7 - col));
                        int y = (spriteY * 8 + row);
                        */
        }
    }

    void PPU::setPixel(int x, int y, u32 color) {
        pixelData[(y * pixelWidth) + x] = color;
    }

    void PPU::incrementX() {
        // If rendering is activiated.
        if (ppuMask.renderBackground || ppuMask.renderSprites) {
            // If we get too the end
            if (vram.coarseX == 31) {
                // If we get to the end, go back to the begining of the
                // NEXT nametable
                vram.coarseX = 0;
                vram.nameTableX = ~vram.nameTableX;
            }
            else {
                vram.coarseX++;
            }
        }
    }

    void PPU::incrementY() {
        // Icrement fineY
        if (vram.fineY < 7) {
            vram.fineY++;
        }
        else {
            vram.fineY = 0; // Reset fine y then reset coarse. flip table if scrolling.
            if (vram.coarseY == 31) {
                vram.coarseY = 0;
                vram.nameTableY = ~vram.nameTableY;
            }
            else {
                vram.coarseY++;
            }
        }
    }

    void PPU::loopyTransferX() {
        if (ppuMask.renderBackground || ppuMask.renderSprites) {
            vram.nameTableX = tempVram.nameTableX;
            vram.coarseX = tempVram.coarseX;
        }
    }

    void PPU::loopyTransferY() {
        if (ppuMask.renderBackground || ppuMask.renderSprites) {
            vram.nameTableY = tempVram.nameTableY;
            vram.coarseY = tempVram.coarseY;
            vram.fineY = tempVram.fineY;
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
