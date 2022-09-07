#include "cpu.h"
#include "nesemulator.h"

NESEmulator& NESEmulator::the()
{
    static NESEmulator sEmulator;
    return sEmulator;
}


NESEmulator::NESEmulator()
{
    mCPU.reset({});
}
