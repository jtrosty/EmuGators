#include "bus.h"

Bus::Bus() {
    // Initialize memory to zero
    memory = new u8[0xFFFF + 1] {0};
}

Bus::~Bus() {
    delete[] memory;
}

void Bus::loadROM(QByteArray rom) {
    for (int i = 0; i < rom.size(); i++) {
        if (ramStart + i >= 0xFFFA) {
            qInfo("Rom load violated address Rom space");
        }
        memory[ramStart + i] = rom.at(i);
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

void Bus::writeMemory(u16 addr, u8 data) {
    if ( addr < ramMirror) {
        addr = addr & 0x1FFF;
    }
    else if (addr >= ppuRegisterStart && addr <= ppuRegisterEnd) {
        addr = addr & ppuMirror;
    }
    memory[addr] = data;
}