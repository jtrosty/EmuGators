#include "ppu.h"

PPU::PPU(QByteArray* glPixelArray, Bus* _bus) {
    pixelData = glPixelArray;
    bus = _bus;
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
    bus->writeMemory(address, data);
}

u8 PPU::ppuReadRegister(u16 address) {
    return bus->readMemory(address);
}

void PPU::ppuWriteVRAM(u16 address, u8 data) {
    vRam[address] = data;
}

u8 PPU::ppuReadVRAM(u16 address) {
    return vRam[address];
}

