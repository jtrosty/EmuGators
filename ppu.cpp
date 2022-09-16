#include "ppu.h"

PPU::PPU(QByteArray* glPixelArray, Bus* _bus) {
    pixelData = glPixelArray;
    bus = _bus;
}

PPU::~PPU() {

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

