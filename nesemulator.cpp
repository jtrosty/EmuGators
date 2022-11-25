#include "nesemulator.h"
#include "cpu.h"
#include "ppu.h"

void NESEmulator::powerOn(RomLoader& romLoader)
{
    //NESEmulator::Bus::the().initialize(romLoader.loadTestRom("color_test.nes"));
    //NESEmulator::Bus::the().initialize(romLoader.loadTestRom("test_cpu_exec_space_ppuio.nes"));
    //NESEmulator::Bus::the().initialize(romLoader.debug_getDonkeyKongRom());
    romLoader.loadRom("PacMan.nes");
    //NESEmulator::Bus::the().initialize(romLoader.getRom());
    NESEmulator::Bus::the().initialize(romLoader.nesROM());

    NESEmulator::CPU::the().reset();
}

