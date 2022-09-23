#include "cpu.h"
#include "nesemulator.h"

void NESEmulator::powerOn()
{
    Bus::the().initialize();
    CPU::the().reset();
}

