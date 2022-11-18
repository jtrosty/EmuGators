#include "nesemulator.h"
#include "cpu.h"
#include "ppu.h"

void NESEmulator::powerOn(RomLoader& romLoader)
{
    NESEmulator::Bus::the().initialize(romLoader.debug_getDonkeyKongRom());
    NESEmulator::CPU::the().reset();
}

