#include "bus.h"

Bus::Bus(Badge<NESEmulator>) {
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
    u16 value = readMemory(addr) << 8;
    return value |= readMemory(addr + 1);
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
