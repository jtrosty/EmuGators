#include <stdio.h>
#include <stdlib.h>
#include "cpu.h"
#include "nesemulator.h"

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
	RUN_IF_ALU_OPCODE(ORA);
	RUN_IF_ALU_OPCODE(AND);
	RUN_IF_ALU_OPCODE(EOR);
	RUN_IF_ALU_OPCODE(ADC);
	RUN_IF_ALU_OPCODE(CMP);
	RUN_IF_ALU_OPCODE(SBC);
    default:
	UNDEFINED_INSTRUCTION(inst);
	    
    }
}

void CPU::normallyIncrementClockCycle(MemoryAccessMode mode)
{
    switch (mode) {
    case MemoryAccessMode::Immediate:
    case MemoryAccessMode::Accumulator:
	mClockCycle += 2;
	break;
    case MemoryAccessMode::ZeroPage:
	mClockCycle += 3;
	break;
    default:
	fprintf(stderr, "Cannot increment clock cycle, memory access mode unimplemented.\n");
	exit(1);
    }
}


u8 CPU::getOperand(MemoryAccessMode mode)
{
    auto& emulator = NESEmulator::the();
    switch (mode) {
    case MemoryAccessMode::Accumulator:
       return mA;
    case MemoryAccessMode::Immediate:
	return emulator.bus().readMemory(mPC++);
    case MemoryAccessMode::ZeroPage:
       return emulator.bus().readMemory(emulator.bus().readMemory(mPC++));
    default:
       fprintf(stderr, "Only immediate and zero page memory access are implemented!\n");
       exit(1);
    }
    return mA;
}


void CPU::ORA(MemoryAccessMode mode)
{
    printf("ORA\n");
    u8 operand = getOperand(mode);
    mA |= operand;
}

void CPU::AND(MemoryAccessMode mode)
{
    // just accumulator mode
    printf("AND\n");
    u8 operand = getOperand(mode);
    mA &= operand;
}

void CPU::EOR(MemoryAccessMode mode)
{
    // just accumulator mode
    printf("EOR\n");
    u8 operand = getOperand(mode);
    mA ^= operand;
}

void CPU::ADC(MemoryAccessMode mode)
{
    // just accumulator mode
    printf("ADC\n");
    u8 operand = getOperand(mode);
    u8 result = mA + operand;
    if (result < mA)
	mP = ProcessorStatus::Carry;
    if (mA == 0)
	mP = ProcessorStatus::Zero;
    if (result & 0x10000000)
	mP = ProcessorStatus::Negative;
}

void CPU::CMP(MemoryAccessMode mode)
{
    // just accumulator mode
    printf("CMP\n");
    u8 operand = getOperand(mode);
    
    if (mA > operand)
	mP = ProcessorStatus::Carry;
    if (mA == operand)
	mP = ProcessorStatus::Zero;
    if (operand & 0x10000000)
	mP = ProcessorStatus::Negative;
}

void CPU::SBC(MemoryAccessMode mode)
{
    // just accumulator mode
    printf("SBC\n");
    u8 operand = getOperand(mode);
    u8 result = mA - operand;
    if (result > mA)
	mP = ProcessorStatus::Carry;
    if (mA == 0)
	mP = ProcessorStatus::Zero;
    if (result & 0x10000000)
	mP = ProcessorStatus::Negative;
}
