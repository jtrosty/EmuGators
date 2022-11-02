#include "bus.h"

namespace NESEmulator {

void Bus::initialize()
{
    // Initialize memory to zero
    memory = new u8[0xFFFF + 1] {0};
}

Bus::~Bus() {
    delete[] memory;
}

void Bus::loadROM(QByteArray rom) {
    for (int i = 0; i < rom.size(); i++) {
        if (mRamStart + i >= 0xFFFA) {
            qInfo("Rom load violated address Rom space");
        }
        memory[mRamStart + i] = rom.at(i);
    }
}
void Bus::mattCPUTestLoadROM(QByteArray rom) {
    // Just for now this will assume nrom-128 (last 16kb of rom are a repeat of the first 16kb)
    //bool hasTrainer = rom.at(6) & 0b1000;
    // printf("has trainer: %u\n", hasTrainer);
    unsigned cartridgeIndex = cartridgeROM;
    for (int i = 16; i < kib(16) + 16; i++, cartridgeIndex++) {
        if (cartridgeROM + i >= 0xFFFA) {
            qInfo("Rom load violated address Rom space");
        }
        memory[cartridgeIndex] = rom.at(i);
    }
    //printf("cartridge first load ends at %08x\n", cartridgeIndex);
    for (int i = 16; i < kib(16) + 16; i++, cartridgeIndex++) {
        if (cartridgeROM + i >= 0xFFFA) {
            qInfo("Rom load violated address Rom space");
        }
        memory[cartridgeIndex] = rom.at(i);
    }
}

u8 Bus::readMemory(u16 addr) {
        // Ram Mirroring
    if ( addr < ramMirror) {
        addr = addr & ramMirror;
    }
        // PPU Mirroring
    else if (addr >= ppuRegisterStart && addr <= ppuRegisterEnd) {
        addr = addr & ppuMirror;
    }
    return memory[addr];
}

u16 Bus::readMemory16Bits(u16 addr) {
    // lmao
    u16 value = readMemory(addr);
    return value | (readMemory(addr + 1) << 8);
}

void Bus::writeMemory16Bits(u16 addr, u16 data) {
    writeMemory(addr, data & 0xff);
    writeMemory(addr + 1, data >> 8);
}


void Bus::writeMemory(u16 addr, u8 data) {
    if ( addr < ramMirror) {
        addr = addr & 0x1FFF;
    }
    else if (addr >= ppuRegisterStart && addr <= ppuRegisterEnd) {
        addr = addr & ppuMirror;
    }
    memory[addr] = data;
}

}

using NESEmulator::Bus;
