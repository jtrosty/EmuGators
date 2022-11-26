#include "bus.h"
#include "ppu.h"
#include "cpu.h"
//#include "romloader.h"

namespace NESEmulator {

void Bus::initialize(QByteArray romToLoad)
{
    // Initialize memory to zero
    romLoaded = romToLoad;
    memory = new u8[0xFFFF + 1] {0};
    mattCPUTestLoadROM(romToLoad);


    // DEBUG memory and isntruciton code issues:
    u16 startOfPrgRom = 16;
    for (int i = 0; i < 0x3FFF; i++) {
        debug_prgRom[i] = romToLoad.at(startOfPrgRom + i);
    }
    pDMA = &memory[0x4014];
}

Bus::~Bus() {
    delete[] memory;
}

void Bus::execLoop() {

    // The CPU runs slowers and only goes once very 3 PPU cycles.
    if (clockCycle % 3 == 0) {
        // If DMA is happening, normal CPU operations stop
        if (performDMA) {
            // Transfer only starts on odd clock cycles.
            if (startTransferDMA) {
                // We have started the DMA process.
                if (clockCycle % 2 == 0) {
                    // Get data on even
                    u16 tAddr = (*pDMA << 8);
                    tAddr = tAddr | dmaAddr;
                    dataDMA = readMemory(tAddr);
                }
                else {
                    // Copy data to dma on odd
                    PPU::the().pOAM[dmaAddr] = dataDMA;
                    dmaAddr++;
                    // Once we have done a full 256 bytes of transfer we are done
                    if(dmaAddr == 0x00) {
                        performDMA = false;
                        startTransferDMA = false;
                    }
                }
            }
            else if ((clockCycle % 2 == 1) && (!startTransferDMA)) {
                // Start DMA
                startTransferDMA = true;
            }

            // then on 1 cycle it transfers memory and hte other it
        }
        else {
            NESEmulator::CPU::the().step(1);
        }
    }

    // The ppu runs a cycle every loop
    PPU::the().executeLoop();


    // NMI control
    if (PPU::the().getNMI() > 0) {
        // Set NMI to false;
        PPU::the().setNMItoZero();
        CPU::the().NMI();
    }
    clockCycle++;
}


void Bus::loadROM(QByteArray rom) {
    for (int i = 0; i < rom.size(); i++) {
        if (mRamStart + i >= 0xFFFA) {
            qInfo("Rom load violated address Rom space");
        }
        memory[cartridgeROM + i] = rom.at(i);
    }
    u8 numOfRomBanks = rom.at(4);
    u8 numOfVramBlocks = rom.at(5);
    u8 is512Trainer = rom.at(6) & 0b00000010;
    u16 chrRomStart = 16;
    if (is512Trainer) {
        chrRomStart += 512;
    }
    chrRomStart += (0x4000 * numOfRomBanks);
    PPU::the().loadVram(rom, rom.at(5), chrRomStart);
}

void Bus::loadROM() {
    for (int i = 0; i < romLoaded.size(); i++) {
        if (mRamStart + i >= 0xFFFA) {
            qInfo("Rom load violated address Rom space");
        }
        memory[cartridgeROM + i] = romLoaded.at(i);
    }
    u8 numOfRomBanks = romLoaded.at(4);
    u8 numOfVramBlocks = romLoaded.at(5);
    u8 is512Trainer = romLoaded.at(6) & 0b00000010;
    u16 chrRomStart = 16;
    if (is512Trainer) {
        chrRomStart += 512;
    }
    chrRomStart += (0x4000 * numOfRomBanks);
    PPU::the().loadVram(romLoaded, numOfVramBlocks, chrRomStart);
}

void Bus::mattCPUTestLoadROM(QByteArray rom) {
    // Just for now this will assume nrom-128 (last 16kb of rom are a repeat of the first 16kb)
    //bool hasTrainer = rom.at(6) & 0b1000;
    // printf("has trainer: %u\n", hasTrainer);
    unsigned cartridgeIndex = cartridgeROM;
    for (int i = 16; i < kib(16) + 16; i++, cartridgeIndex++) {
        if (cartridgeROM + i >= 0xFFFF) {
            qInfo("Rom load violated address Rom space");
        }
        memory[cartridgeIndex] = rom.at(i);
    }
    //printf("cartridge first load ends at %08x\n", cartridgeIndex);
    for (int i = 16; i < kib(16) + 16; i++, cartridgeIndex++) {
        if (cartridgeROM + i >= 0xFFFF) {
            qInfo("Rom load violated address Rom space");
        }
        memory[cartridgeIndex] = rom.at(i);
    }
    u8 numOfRomBanks = (u8)rom.at(4);
    //numOfRomBanks = 1;
    u8 numOfVramBlocks = (u8)rom.at(5);
    //numOfVramBlocks = 1;
    u8 is512Trainer = rom.at(6) & 0b00000010;
    u16 chrRomStart = 16;
    if (is512Trainer) {
        chrRomStart += 512;
    }
    chrRomStart += (0x4000 * numOfRomBanks);
    // Set mirroring
    if ((rom.at(6) & 0x01) == 1) {
        PPU::the().setMirroring(1);
    }
    else {
        PPU::the().setMirroring(0);
    }
    NESEmulator::PPU::the().loadVram(rom, numOfVramBlocks, chrRomStart);
}


u8 Bus::readMemory(u16 addr) {
        // Ram Mirroring
    /*
    if (addr >= cartridgeROM && addr <= 0xFFFF) {
        return romLoaded.at(addr & 0x3FFF);
    }
    */
    if ( addr < ramMirror) {
        addr = addr & ramMirror;
    }
        // PPU Mirroring
    else if (addr >= ppuRegisterStart && addr <= ppuRegisterEnd) {
        return NESEmulator::PPU::the().ppuReadRegister(addr);
    }
    else if (addr >= ppuIOStart && addr < apuControlIOStart) {

    }
    else if (addr >= 0x4016 && addr <= 0x4017) {
	u8 data = (mControllerCache[addr & 1] & 0x80) > 0;
	mControllerCache[addr & 1] <<= 1;
	return data;
    }
    if (addr >= cartridgeROM) {
        addr = (addr & 0x3FFF);
        return (debug_prgRom[addr]);//& 0x3FFF));
    }
    return memory[addr];
}

u16 Bus::readMemory16Bits(u16 addr) {
    // lmao
    u16 valueLo = readMemory(addr);
    u16 valueHi = readMemory(addr + 1);
    u16 result = valueLo | (valueHi << 8);
    return result;
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
        addr = (addr & 0x0007);
        NESEmulator::PPU::the().ppuWriteRegister(addr, data);
    }
    else if (addr >= 0x4016 && addr <= 0x4017) {
        mControllerCache[addr & 1] = mController[addr & 1];
        return;
    }
    // DMA Transfer
    else if (addr == 0x4014) {
        *pDMA = data;
        dmaAddr = 0x00;
        performDMA = true;
        startTransferDMA = false;
    }

    memory[addr] = data;
}

}

using NESEmulator::Bus;
