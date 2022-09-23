#include "ppu.h"

namespace NESEmulator {

void PPU::initialize(QByteArray* glPixelArray) {
    pixelData = glPixelArray;
}

PPU::~PPU() {

}

void PPU::drawSprite(int x, int y) {
    for (int i = nameTableStart; i < nameTableStart + 0x0400; i++) {
        u8 spriteToUse = vRam[i];
        u8 spriteAddress = vRam[spriteStart + spriteToUse];
    }
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

}

