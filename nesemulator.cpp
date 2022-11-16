#include "nesemulator.h"
#include "cpu.h"
#include "ppu.h"

void NESEmulator::powerOn()
{
    Bus::the().initialize();
    CPU::the().reset();
}

void NESEmulator::execLoop() {

    // The ppu runs a cycle every loop
    PPU::the().executeLoop();

    // The CPU runs slowers and only goes once very 3 PPU cycles.
    if (clockCycle % 3 == 0) {
        CPU::the().execLoop();
    }

    // NMI control
    if (Bus::the().readMemory(addrNMI)) {
        // TODO do stuff?? or is it already handled

    }
}

