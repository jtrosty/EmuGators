#include "ppu.h"

namespace NESEmulator {

#define PRINT_NT_Palette 1

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
            tempVramLoopy.nameTableX = ppuControl.scrollingNametableX;
            tempVramLoopy.nameTableY = ppuControl.scrollingNametableY;
            break;
        }
        case 0x0001: {
            ppuMask.reg = data;
            //ppuMask.reg = 0x0F;
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
            oamAddress = data;

            break;
        }
        case 0x0004: {
            // Can be read in rare instances
            // OAM data
            // DO NOT WRITEE DIRECTLY TO THIS REGISTER IN MOST CASES PER NESDEV
            pOAM[oamAddress] = data;
            break;
        }
        case 0x0005: {
            // ppuScrol Tells ppu which pixel of nametable selected through ppuctr
            // should be at the top left corner of rendered screen
            // Typically written during vertical blanking. Change made during rendering wont take
            // affect till the next grame.
            // Write only
            if (ppuAddressLatch == 1) {
                tempVramLoopy.fineY = data & 0x07;
                tempVramLoopy.coarseY = data >> 3;
                ppuAddressLatch = 0;
            }
            else if (ppuAddressLatch == 0) {
                fineX = data & 0x07;
                tempVramLoopy.coarseX = data >> 3;
                ppuAddressLatch = 1;
            }
            break;
        }
        case 0x0006: {
            // Write only
            // ppu addess, write only
            if (ppuAddressLatch == 1) {
                tempVramLoopy.reg = (tempVramLoopy.reg & 0xFF00) | (u16)data;
                vramLoopy = tempVramLoopy;
                ppuAddressLatch = 0;
            }
            else if (ppuAddressLatch == 0) {
                tempVramLoopy.reg = (tempVramLoopy.reg & 0x00FF) | (((u16)(data & 0x3F)) << 8);
                ppuAddressLatch = 1;
            }
            break;
        }
        case 0x0007: {
            // VRAM read/write data register
            ppuWriteVRAM(vramLoopy.reg, data);
            // Increment the nametable addresw

            // Depends on if in horizontal mode or veritcal mode.  Logic not yet yet
            // TODO: Need logic for if veritcal or horizontalmode
            if (ppuControl.VramAddressIncrement) {
                // If vertical mode
                vramLoopy.reg += 32;
            }
            else {
                // if horizontal mode
                vramLoopy.reg += 1;
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
            // not readable
            // result = ppuControl.reg;
            break;
        }
        case 0x0001: {
            // Not read able
            // result = ppuMask.reg;
            break;
        }
        case 0x0002: {
            //ppuStatus.verticalBlank = 1;
            //ppuStatus.reg = 0xFF;
            //ppuDATA = 0xFF;
            result = (ppuStatus.reg & 0b11100000); //| (ppuDATA & 0x00011111);

            ppuStatus.verticalBlank = 0;
            ppuAddressLatch = 0;
            // TODO(Jon): Clear latch by PPUSCROL and PPUADDR.
            break;
        }
        case 0x0003: {
            // Does not get read
            //OAM - Object Attribute Memory address
        }
        case 0x0004: {
            // Can be read in rare instances
            // OAM data
            result = pOAM[oamAddress];
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
            // the load the ppuData with the next byte
            ppuDATA = ppuReadVRAM(vramLoopy.reg);

            if (vramLoopy.reg < 0x3F00) {
                // Nothing? delete
            }
            else if (vramLoopy.reg >= 0x3F00) {
                result = ppuDATA;
            }
            // Increment the nametable addressw

            // Depends on if in horizontal mode or veritcal mode.  Logic not yet yet
            // TODO: Need logic for if veritcal or horizontalmode
            if (ppuControl.VramAddressIncrement) {
                // If vertical mode
                vramLoopy.reg += 32;
            }
            else {
                // if horizontal mode
                vramLoopy.reg += 1;
            }

            break;
        }
        default: {
            printf("Accessed read ppu registers, but got default. Index value %i", index);
            break;
        }
        }
        return result;
    }

    void PPU::ppuWriteVRAM(u16 address, u8 data) {
        // Name table space
        if (address >= 0x2000 && address <= 0x2EFF) {
            //vertical
            if (mirroring == 1) {
                // Name table 0 and table 2, map to table 0
                if ((address >= 0x2000 && address <= 0x23FF) ||
                    (address >= 0x2800 && address <= 0x2BFF)) {

                    address = 0x2000 + (address & 0x0FFF);
                }
                // Name table 1 and 3 mapped onto trable 1
                else if ((address >= 0x2400 && address <= 0x27FF) ||
                        (address >= 0x2C00 && address <= 0x2FFF)) {
                    address = 0x2400 + (address & 0x0FFF);
                }
            }
            // Horizontal
            else if (mirroring == 0) {
                // Name table 0 and table 1, map to table 0
                if ((address >= 0x2000 && address <= 0x23FF) ||
                    (address >= 0x2400 && address <= 0x27FF)) {

                    address = 0x2000 + (address & 0x0FFF);
                }
                // Name table 2 and 3 mapped onto trable 2
                else if ((address >= 0x2800 && address <= 0x2BFF) ||
                        (address >= 0x2C00 && address <= 0x2FFF)) {
                    address = 0x2800 + (address & 0x0FFF);
                }
            }
        }
        else if (address >= 0x3F00 && address <= 0x3FFF) {
            address &= 0x001F;
            if (address == 0x0010) address = 0x0000;
            if (address == 0x0014) address = 0x0004;
            if (address == 0x0018) address = 0x0008;
            if (address == 0x001C) address = 0x000C;
            address = 0x3F00 | address;
        }
        vRam[address] = data;
    }
    /*
        if (address >= 0x3000 && address <= 0x3EFF) {
            address = 0x2000 + (address & 0x0FFF);
            // nametable 2 (the thrid one)
            if (address >= 0x2800 && address <= 0x2BFF) {
                address = 0x2000 + (address & 0x03FF);
            }
            else if (address >= 0x2C00 && address <= 0x2FFF) {
                address = 0x2400 + (address & 0x03FF);
            }
        }
        if (address >= 0x2000 && address <= 0x2EFF) {
            // Name table 1
            if ((address >= 0x2000 && address <= 0x23FF) ||
                 (address >= 0x2800 && address <= 0x2BFF)) {
                address = 0x2000 + (address & 0x0FFF);
            }
            if ((address >= 0x2400 && address <= 0x27FF) ||
                 (address >= 0x2C00 && address <= 0x2FFF)) {
                address = 0x27FF + (address & 0x0FFF);
            }
        }
        */

    u8 PPU::ppuReadVRAM(u16 address) {
        u8 result = 0x00;
        if (address >= 0x2000 && address < 0x3EFF) {
            if (address >= 0x3000 && address <= 0x3EFF) {
                address = 0x2000 + (address & 0x0FFF);
            }
            //vertical
            if (mirroring == 1) {
                // Name table 0 and table 2, map to table 0
                if ((address >= 0x2000 && address <= 0x23FF) ||
                    (address >= 0x2800 && address <= 0x2BFF)) {

                    address = 0x2000 + (address & 0x03FF);
                }
                // Name table 1 and 3 mapped onto trable 1
                else if ((address >= 0x2400 && address <= 0x27FF) ||
                        (address >= 0x2C00 && address <= 0x2FFF)) {
                    address = 0x2400 + (address & 0x03FF);
                }
            }
            // Horizontal
            else if (mirroring == 0) {
                // Name table 0 and table 1, map to table 0
                if ((address >= 0x2000 && address <= 0x23FF) ||
                    (address >= 0x2400 && address <= 0x27FF)) {

                    address = 0x2000 + (address & 0x03FF);
                }
                // Name table 2 and 3 mapped onto trable 2
                else if ((address >= 0x2800 && address <= 0x2BFF) ||
                        (address >= 0x2C00 && address <= 0x2FFF)) {
                    address = 0x2800 + (address & 0x03FF);
                }
            }
        }
        else if (address >= 0x3F00 && address <= 0x3FFF) {
            address &= 0x001F;
            if (address == 0x0010) address = 0x0000;
            if (address == 0x0014) address = 0x0004;
            if (address == 0x0018) address = 0x0008;
            if (address == 0x001C) address = 0x000C;
            address = 0x3F00 | address;
        }
        result = vRam[address];
        return result;
    }

    u8 PPU::getPalette(u16 address) {
        u8 result = 0x00;
        return result;
    }

    void PPU::loadVram(QByteArray &rom, u8 num8kVram, u16 chrRomStart) {
        u32 chrRomSize = num8kVram * 0x2000;
        u32 romSize = rom.size();
        for (int i = 0; i < chrRomSize; i++) {
            vRam[i] = rom[chrRomStart + i];
        }
        /*
        for (int i = 0x3F00; i < 0x3F10; i++) {
            vRam[i] = 5;
        }
        */

#if PRINT_NT_Palette
        for (int i = 0x0000; i < 0x23FF; i++) {
            qDebug() << i << ": 0x" << vRam[i] << " | ";
        }
#endif

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
        u16 localAddress = 0x0000;
        if (scanline >= -1 && scanline < 240) {

            // This cycle is skpped for scanline 0
            if (scanline == 0 && cycle == 0) {
                cycle = 1;
            }
            if (scanline == -1 && cycle == 1) {
                ppuStatus.verticalBlank = 0;
            }

            if ((cycle >= 2 && cycle < 258) || (cycle >= 321 && cycle < 338)) {

                if (ppuMask.renderBackground) {
                    // Shift bits after 1 cycle
                    patternTableShifterHi <<= 1;
                    patternTableShifterLow <<= 1;
                    palleteShifterHi <<= 1;
                    palleteShifterLow <<= 1;
                }

                // during visible seciton every 8 cycles updatessome variable to be used on the next cycle.
                // 1 cycle per pixel
                // It takes 2 cycles to write a 16-bit word.
                // other other case will be skipped.
                switch ((cycle - 1) % 8) {
                // NT byte
                case 0:
                    // This will set the currnet shifter which will be used below.
                    setCurrentShifter();

                    // Load the bgNext name table for the following 8 cycles
                    localAddress = 0x2000 | (vramLoopy.reg & 0x0FFF);
                    bgNextNametableValue = ppuReadVRAM(localAddress);

                    break;
                case 2:

                    // this is the port of the prepatory phase and gets the next nametable tile.
                    // the address starts tt VRAM and then uses the LSB of vramLoopy.reg to get the correct byte.
                    // the 12 bytes are, course x (5 bits), and course y (5 bits), with the name table values as the MSBs.
                    // eac                   // To get to the attribute, 12 bits total to get to the attribute
                    // from NESDEV.org:
                    // NN 1111 YYY XXX
                    // || |||| ||| +++-- high 3 bits of coarse X (x/4)
                    // || |||| +++------ high 3 bits of coarse Y (y/4)
                    // || ++++---------- attribute offset (960 bytes)
                    // ++--------------- nametable select
                    // For background tiles, the last 64 bytes of each nametable
                    // are reserved for assigning a specific palette to a part of the background.
                    // This section is called an attribute table.
                    //course x and y are divided by 4,(shifted 2) because
                    localAddress = 0x23C0 | (vramLoopy.nameTableY << 11) | (vramLoopy.nameTableX << 10) |
                                            (vramLoopy.coarseX >> 2) | ((vramLoopy.coarseY >> 2) << 3);
                    bgNextTileAttribute = ppuReadVRAM(localAddress);

                                    //0b001111000000                                |
                    // TODO (Jon): I think there is more here
                    // Now we ahve bgNextTileAttribute, this bye of pallete data, covers a 4x4 block of
                    // 0b 11 01 10 00 <--- each 2 bits ared for a the TL, TR, BL, bR of a a group of 4x4 blocks.
                   // find out if we are in TL, TR, BL, BR
                    topOrBottom = vramLoopy.coarseY % 4;
                    leftOrRight = vramLoopy.coarseX % 4;
                    if (topOrBottom < 2 ) { // we are on top
                        if (leftOrRight < 2) { // We are on left
                            // TL
                            bgNextTileAttribute = ((bgNextTileAttribute & 0b00000011));
                        }
                        else {
                            // TR
                            bgNextTileAttribute = ((bgNextTileAttribute & 0b00001100) >> 2);
                        }
                    }
                    else {
                        if (leftOrRight < 2) { // We are on left
                            // BL
                            bgNextTileAttribute = ((bgNextTileAttribute & 0b00110000) >> 4);
                        }
                        else {
                            // BR
                            bgNextTileAttribute = ((bgNextTileAttribute & 0b11000000) >> 6);
                        }
                    }
                    bgNextTileAttribute &= 0b00000011;
                    break;
                case 3:
                    // Skip, it takes 2 cycles to perform the task above
                    break;
                // Pattern Table LSB
                case 4:
                    //
                    // 0000 0000 0000 0000
                    //    X ---- ---- ---- Pattern table selector
                    //      XXXX XXXX ---- This is the nametable value,This byte selects 1 of 256 different patters on the table
                    //                -XXX the fine control called fineY, only 3 bits, 0-7
                    localAddress = (ppuControl.backgroundPatternTable << 12) + ((u16)bgNextNametableValue << 4) + vramLoopy.fineY + 0x0000;
                    bgPatternLSB = ppuReadVRAM(localAddress);
                    break;
                case 5:
                    // Skip, it takes 2 cycles to perform the task above
                    break;
                // Pattern Table MSB
                case 6:
                    localAddress = (ppuControl.backgroundPatternTable << 12) + ((u16)bgNextNametableValue << 4)+ vramLoopy.fineY + 0x0008;
                    bgPatternMSB = ppuReadVRAM(localAddress);
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
                    setCurrentShifter();
                    loopyTransferX();
                }
                if (cycle == 338 || cycle == 340) {
                    // The case 1-8 repeats from above after 280, but no more pixels are displayed to
                    // The screen, this below does nothign
                    bgNextNametableValue =  ppuReadVRAM(0x2000 | (vramLoopy.reg & 0x0FFF));
                }
                if (scanline == -1 && cycle >= 280 && cycle < 305) {
                    // We have completed the row, in preperation for the next, perform
                    // Loopy register y transform from tvramLoopy to vramLoopy.
                    loopyTransferY();
                }
            }
        }

        else if ( scanline >= 241 && scanline <= 260) {
            if (scanline == 241 && cycle == 1) {
                // The frame is done.
                ppuStatus.verticalBlank = 1;
                // tell CPU tyhat rendering is complete
                if (ppuControl.NMI) {
                    // Set NMI in RAM to true
                    NMIFlag = 1;
                }
            }
        }

        u16 colorAddress = 0x3F00;
        u32 pixelColor = ppuReadVRAM(0x3F00);
        if (ppuMask.renderBackground) {
            // Perform redner of background,
            // Only 1 bit is needed that correlates with 0-7 based on wehre int eh cycle we are.
            u16 mask = 0x8000 >> fineX;  //https://docs.google.com/document/d/1o9N4FHd5cBQrIEk_3XJsgFpvgkrLlJYZ4u9N1GTJgcc/edit
            // Shift bits 1, 1 bit per pixel
            //bgPatternLSB <<= 1;
            //bgPatternMSB <<= 1;

            u16 lowerBit = patternTableShifterHi & mask;
            u16 higherBit = patternTableShifterLow & mask;
            // Only need to knwo if the value is 1 or 0.
            if (lowerBit > 0) lowerBit = 0x0001;
            if (higherBit > 0) higherBit = 0x0001;
            // this pixel value gives you 0-4 and sets the color in the pallete
            u16 pixelColorValue = lowerBit  | (higherBit << 1);
            // Now get the pallete that will be used

            u16 palleteLowBit = palleteShifterLow & mask;
            u16 palleteHighBit = palleteShifterHi & mask;
            if (palleteLowBit > 0) palleteLowBit = 0x01;
            if (palleteHighBit > 0) palleteHighBit = (0x01 << 1);
            u16 palleteValue = palleteLowBit | palleteHighBit;

            colorAddress = (ppuReadVRAM(0x3F00 + (palleteValue << 2) + pixelColorValue)) & 0x3F; //% 63;
            pixelColor = colors[colorAddress];
        }


        if (scanline >= 0 && scanline < 240 && cycle < 256 ) {
            setPixel(cycle - 1, scanline, pixelColor);
        }
        //setPixel(cycle - 1, scanline, pixelColor);


        cycle++;
        if (cycle >=341) {
            cycle = 0;
            scanline++;
            if (scanline >= 261) {
                scanline = -1;

#if PRINT_NT_PALETTE
                for (int i = 0x2000; i < 0x23FF; i++) {
                    qDebug() << i << ": 0x" << vRam[i] << " | ";
                }
                qDebug() << "Palette ##########################################";
                for (int i = 0x3f00; i < 0x3f10; i++) {
                    qDebug() << i << ": 0x" << vRam[i] << " | ";
                }
#endif
            }
        }
    }

    void PPU::setCurrentShifter() {
        // because it shifts 1 bit to the left everycycle, we gett the top byte and load the bottom byte
        // with the Next byte NametableValue

        patternTableShifterHi =  (patternTableShifterHi & 0xFF00) | bgPatternMSB;
        patternTableShifterLow =  (patternTableShifterLow & 0xFF00) | bgPatternLSB;


        // doing the same thing with the atribute shifters. ONly
        u8 whichPallete = 0x00;
        if ((bgNextTileAttribute & 0x10) > 0) whichPallete = 0xFF;
        else 								  whichPallete = 0x00;

        palleteShifterHi =  (palleteShifterHi & 0xFF00) | whichPallete;

        whichPallete = 0x00;
        if ((bgNextTileAttribute & 0x01) > 0) whichPallete = 0xFF;
        else 								  whichPallete = 0x00;
        palleteShifterLow =  (palleteShifterLow & 0xFF00) | whichPallete;
    }


    void PPU::setPixel(u32 x, u32 y, u32 color) {
        u32 index = (y * 256) + x;
        pixelData[index] = color;
        /*
        for (int i = 0; i < 1000; i++) {
            pixelData[i] = 0xFF00FFFF;
        }
        */
     }

    void PPU::incrementX() {
        // If rendering is activiated.
        if (ppuMask.renderBackground || ppuMask.renderSprites) {
            // If we get too the end
            if (vramLoopy.coarseX >= 31) {
                vramLoopy.coarseX = 0;
                // If we get to the end, go back to the begining of the
                // NEXT nametable vramLoopy.coarseX = 0;
                // The nametable bit flips to switch to the other nametable.
                vramLoopy.nameTableX = ~vramLoopy.nameTableX;
            }
            else {
                vramLoopy.coarseX++;
            }
        }
    }

    void PPU::incrementY() {
        // Icrement fineY
        if (ppuMask.renderBackground || ppuMask.renderSprites) {
            if (vramLoopy.fineY < 7) {
                vramLoopy.fineY++;
            }
            else {
                vramLoopy.fineY = 0; // Reset fine y then reset coarse. flip table if scrolling.
                if (vramLoopy.coarseY == 29) {
                vramLoopy.coarseY = 0;
                vramLoopy.nameTableY = ~vramLoopy.nameTableY;
                }
                else if (vramLoopy.coarseY >= 31) {
                vramLoopy.coarseY = 0;
                }
                else {
                vramLoopy.coarseY++;
                }
            }
        }
    }

    void PPU::loopyTransferX() {
        if (ppuMask.renderBackground || ppuMask.renderSprites) {
            vramLoopy.nameTableX = tempVramLoopy.nameTableX;
            vramLoopy.coarseX = tempVramLoopy.coarseX;
        }
    }

    void PPU::loopyTransferY() {
        if (ppuMask.renderBackground || ppuMask.renderSprites) {
            vramLoopy.nameTableY = tempVramLoopy.nameTableY;
            vramLoopy.coarseY = tempVramLoopy.coarseY;
            vramLoopy.fineY = tempVramLoopy.fineY;
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
