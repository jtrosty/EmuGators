#include <stdio.h>
#include <stdlib.h>
#include "cpu.h"

void CPU::reset(Badge<NESEmulator>)
{
    mP = (ProcessorStatus)0x34; // Why 0x34?
    mA = mX = mY = 0;
    mSP = 0xfd;
    mClockCycle = 0;
}

void CPU::runOpcode(EncodedInstructionType inst)
{
    printf("runOpcode happens with %u\n", (unsigned)inst);
    
    switch (inst) {
	RUN_IF_ALU_OPCODE(AND)
    }
}


void CPU::AND(MemoryAccessMode mode)
{
    // just accumulator mode
    printf("This happens\n");
    switch (mode) { // TODO: A large switch statement to check for the access mode will not do down the line. Too much duplication.
    case MemoryAccessMode::Immediate:
	mA &= mA;
	break;
    default:
	UNDEFINED_MEMORY_ACCESS_MODE(mode);
    }
}
