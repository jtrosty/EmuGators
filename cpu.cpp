#include <stdio.h>
#include <stdlib.h>
#include "cpu.h"
#include "nesemulator.h"

namespace NESEmulator {

void CPU::reset()
{
    mP = (ProcessorStatus)0x34; // Why 0x34?
    mA = mX = mY = 0;
    mSP = 0xfd;
    mIsRunning = true;
    mPC = 0xc000; //Bus::the().ramStart();
    mClockCycle = 0;
}

void CPU::setProcessorStatus(ProcessorStatus status)
{
    mP = (ProcessorStatus)((unsigned)mP | (unsigned)status);
}
void CPU::clearProcessorStatus(ProcessorStatus status)
{
    mP = (ProcessorStatus)((unsigned)mP & (0b11111111 ^ (unsigned)status));
}

void CPU::setOrClearStatusIf(bool cond, ProcessorStatus status)
{
    if (cond)
	setProcessorStatus(status);
    else
	clearProcessorStatus(status);
}

void CPU::runOpcode(EncodedInstructionType inst)
{
    switch (inst) {
	IS_OPCODE(BRK, 0x0, Implicit);
	IS_OPCODE(PHA, 0x4e, Implicit);
	IS_OPCODE(JMP, 0x4c, Absolute);
	IS_OPCODE(CLV, 0xb8, Implicit);
	IS_OPCODE(CLC, 0x18, Implicit);
	IS_OPCODE(CLD, 0xd8, Implicit);
	IS_OPCODE(RTS, 0x60, Implicit);
	IS_OPCODE(LDY, 0xa0, Immediate);
	IS_OPCODE(LDX, 0xa2, Immediate);
	IS_OPCODE(JSR, 0x20, Absolute);
	IS_OPCODE(NOP, 0xea, Implicit);
	IS_OPCODE(SEC, 0x38, Implicit);
	IS_OPCODE(SEI, 0x78, Implicit);
	IS_OPCODE(SED, 0xF8, Implicit);
	IS_OPCODE(PHP, 0x08, Implicit);
	IS_OPCODE(PLA, 0x68, Implicit);
	IS_OPCODE(PHA, 0x48, Implicit);
	IS_OPCODE(PLP, 0x28, Implicit);
	IS_OPCODE(BCS, 0xb0, Relative);
	IS_OPCODE(BCC, 0x90, Relative);
	IS_OPCODE(BEQ, 0xf0, Relative);
	IS_OPCODE(BNE, 0xd0, Relative);
	IS_OPCODE(BIT, 0x24, ZeroPage);
	IS_OPCODE(BIT, 0x2c, Absolute);
	IS_OPCODE(BVS, 0x70, Relative);
	IS_OPCODE(BVC, 0x50, Relative);
	IS_OPCODE(BPL, 0x10, Relative);
	IS_OPCODE(BMI, 0x30, Relative);
	IS_OPCODE(CPY, 0xc0, Immediate);
	IS_OPCODE(CPX, 0xe0, Immediate);
	IS_OPCODE(STY, 0x84, ZeroPage);
	IS_OPCODE(STX, 0x86, ZeroPage);
	IS_OPCODE(INY, 0xc8, Implicit);
	IS_OPCODE(INX, 0xe8, Implicit);
	IS_OPCODE(DEY, 0x88, Implicit);
	IS_OPCODE(DEX, 0xca, Implicit);
	IS_OPCODE(STX, 0x8e, Absolute);
	IS_OPCODE(LDX, 0xae, Absolute);
	IS_OPCODE(TAX, 0xaa, Implicit);
	IS_OPCODE(TAY, 0xa8, Implicit);
	IS_OPCODE(TSX, 0xba, Implicit);
	IS_OPCODE(TXA, 0x8a, Implicit);
	IS_OPCODE(TXS, 0x9a, Implicit);
	IS_OPCODE(TYA, 0x98, Implicit);
	IS_OPCODE(LSR, 0x56, XZeroPageIndexed);
	IS_OPCODE(ASL, 0x16, XZeroPageIndexed);
	RUN_IF_ALU_OPCODE(ORA);
	RUN_IF_ALU_OPCODE(AND);
	RUN_IF_ALU_OPCODE(EOR);
	RUN_IF_ALU_OPCODE(ADC);
	RUN_IF_ALU_OPCODE(LDA);
	RUN_IF_ALU_OPCODE(CMP);
	RUN_IF_ALU_OPCODE(SBC);
	RUN_IF_ALU_OPCODE(STA);
	
    default:
	UNDEFINED_INSTRUCTION(inst);
	    
    }
}

void CPU::execLoop()
{
    auto& bus = Bus::the();

    while (mIsRunning) {
	printf("Program counter: %08x, Instruction: %08x\n", mPC, bus.rawMemory()[mPC]);
	runOpcode(EncodedInstructionType(bus.rawMemory()[mPC]));
	
    }
}

void CPU::normallyIncrementClockCycle(MemoryAccessMode mode, u8 timeOffset)
{
    switch (mode) {
    case MemoryAccessMode::Immediate:
    case MemoryAccessMode::Accumulator:
	mClockCycle += 2 + timeOffset;
	break;
    case MemoryAccessMode::ZeroPage:
	mClockCycle += 3 + timeOffset;
	break;
    case MemoryAccessMode::Absolute:
    case MemoryAccessMode::XZeroPageIndexed:
    case MemoryAccessMode::YZeroPageIndexed:
	mClockCycle += 4 + timeOffset;
	break;
    default:
	fprintf(stderr, "Cannot increment clock cycle, memory access mode unimplemented.\n");
	exit(1);
    }
}


u8 CPU::getOperand(MemoryAccessMode mode)
{
    auto& bus = Bus::the();
    switch (mode) {
    case MemoryAccessMode::Accumulator:
       return mA;
    case MemoryAccessMode::Immediate:
	return bus.readMemory(++mPC);
    case MemoryAccessMode::ZeroPage:
	return bus.readMemory(bus.readMemory(++mPC));
    case MemoryAccessMode::Absolute: {
	u8 value = bus.readMemory(bus.readMemory16Bits(mPC));
	mPC += 2;
	return value;
    }
    case MemoryAccessMode::XZeroPageIndexed:
	return bus.readMemory(bus.readMemory(++mPC) + mX);
	break;
    case MemoryAccessMode::YZeroPageIndexed:
	return bus.readMemory(bus.readMemory(++mPC) + mY);
    default:
	fprintf(stderr, "Unimplemented memory access mode number: %u!\n", (unsigned)mode);
       exit(1);
    }
    return mA;
}

u16 CPU::getAddressOperand(MemoryAccessMode mode)
{
    auto& bus = Bus::the();
    switch (mode) {
    case MemoryAccessMode::Accumulator:
       return mA;
    case MemoryAccessMode::Immediate:
    case MemoryAccessMode::ZeroPage:
	return bus.readMemory(++mPC);
    case MemoryAccessMode::Absolute: {
	u16 addr = bus.readMemory16Bits(mPC);
	mPC += 2;
	return addr;
    }
    case MemoryAccessMode::XZeroPageIndexed:
	return bus.readMemory(++mPC) + mX;
    case MemoryAccessMode::YZeroPageIndexed:
	return bus.readMemory(++mPC) + mY;
    default:
       fprintf(stderr, "Only immediate, zero page, Acc, 0X, and 0Y, memory access are implemented!\n");
       exit(1);
    }
    return mA;
}


u8 CPU::decode8Bits()
{
    return Bus::the().readMemory(mPC++);
}
u16 CPU::decode16Bits()
{
    auto val = Bus::the().readMemory16Bits(mPC);
    mPC += 2;
    return val;
}

void CPU::BRK(MemoryAccessMode)
{
    setProcessorStatus(ProcessorStatus::BreakCommand);

    printf("Do I happen as intended at BRK?\n");
    pushWord(mPC + 1); // ?
    pushByte((byte)mP);

    mPC = Bus::the().readMemory16Bits(0xfffe);
    setProcessorStatus(ProcessorStatus::InterruptDisable);
    
    mClockCycle += 7;
    printf("Break happens!\n");
}

void CPU::PHA(MemoryAccessMode)
{
    pushByte(mA);
    mClockCycle += 3;
    mPC++;
}

void CPU::ORA(MemoryAccessMode mode)
{
    printf("ORA\n");
    u8 operand = getOperand(mode);
    mA |= operand;
    setOrClearStatusIf(mA == 0, ProcessorStatus::Zero);
    setOrClearStatusIf(mA & 0b10000000, ProcessorStatus::Negative);
    mPC++;
}

void CPU::AND(MemoryAccessMode mode)
{
    // just accumulator mode
    u8 operand = getOperand(mode);
    printf("AND Operand: %08x, mA: %08x, ", operand, mA);
    mA &= operand;
    printf("Resulting mA: %08x\n", mA);
    setOrClearStatusIf(mA == 0, ProcessorStatus::Zero);
    setOrClearStatusIf(mA & 0b10000000, ProcessorStatus::Negative);
    mPC++;
}

void CPU::EOR(MemoryAccessMode mode)
{
    // just accumulator mode
    u8 operand = getOperand(mode);
    mA ^= operand;
    setOrClearStatusIf(mA == 0, ProcessorStatus::Zero);
    setOrClearStatusIf(mA & 0b10000000, ProcessorStatus::Negative);
    mPC++;
}


// These don't work right! And is it normal for accumulator mode to be like that in other memory things? (or some question like that)
void CPU::LSR(MemoryAccessMode mode)
{
    // just accumulator mode
    u8 operand = getAddressOperand(mode);
    setOrClearStatusIf(mA & 0b1, ProcessorStatus::Carry);
    mA >>= operand;
    setOrClearStatusIf(mA == 0, ProcessorStatus::Zero);
    setOrClearStatusIf(mA & 0b10000000, ProcessorStatus::Negative);
    mPC++;
    //normallyIncrementClockCycle(mode, 2);
}
void CPU::ASL(MemoryAccessMode mode)
{
    // just accumulator mode
    u8 operand = getAddressOperand(mode);
    setOrClearStatusIf(mA & 0b10000000, ProcessorStatus::Carry);
    mA *= operand;
    setOrClearStatusIf(mA == 0, ProcessorStatus::Zero);
    setOrClearStatusIf(mA & 0b10000000, ProcessorStatus::Negative);
    mPC++;
    //normallyIncrementClockCycle(mode, 2);
}

void CPU::ADC(MemoryAccessMode mode)
{
    u8 operand = getOperand(mode);
    
    ADCImpl(operand);
    
    mPC++;
}

void CPU::ADCImpl(u8 operand)
{
    u8 oldValue = mA;
    bool didOverflow = false;
    u8 newValue = oldValue + operand;

    if (newValue < oldValue)
	didOverflow = true;
    u8 oldValue2 = newValue;
    
    newValue += (unsigned)mP & (unsigned)ProcessorStatus::Carry;
    if (newValue < oldValue2)
	didOverflow = true;
    
    mA = newValue;

    setOrClearStatusIf(didOverflow, ProcessorStatus::Carry);

    auto isSignOverflow = [&] () -> bool {
        return (((oldValue & 0x80) == (operand & 0x80)) &&
            ((oldValue & 0x80) != (newValue & 0x80)));
    };
    setOrClearStatusIf(isSignOverflow(), ProcessorStatus::Overflow);
    setOrClearStatusIf(mA == 0, ProcessorStatus::Zero);
    setOrClearStatusIf(mA & 0b10000000, ProcessorStatus::Negative);
}

void CPU::LDA(MemoryAccessMode mode)
{
    u8 operand = getOperand(mode);
    mA = operand;
    printf("This is what LDA got: %08x\n", mA);
    setOrClearStatusIf(mA == 0, ProcessorStatus::Zero);
    setOrClearStatusIf(mA & 0b10000000, ProcessorStatus::Negative);
    normallyIncrementClockCycle(mode);
    mPC++;
}

void CPU::CMP(MemoryAccessMode mode)
{
    // just accumulator mode
    u8 operand = getOperand(mode);
    printf("value of cmp operand: %u\n", operand);
    setOrClearStatusIf(mA >= operand, ProcessorStatus::Carry);
    setOrClearStatusIf(mA == operand, ProcessorStatus::Zero);
    setOrClearStatusIf((mA - operand) & 0b10000000, ProcessorStatus::Negative);
    mPC++;
}
void CPU::CPX(MemoryAccessMode mode)
{
    u8 operand = getOperand(mode);
    printf("value of CPX operand: %u\n", operand);
    setOrClearStatusIf(mX >= operand, ProcessorStatus::Carry);
    setOrClearStatusIf(mX == operand, ProcessorStatus::Zero);
    setOrClearStatusIf((mX - operand) & 0b10000000, ProcessorStatus::Negative);
    mPC++;
}
void CPU::CPY(MemoryAccessMode mode)
{
    u8 operand = getOperand(mode);
    printf("value of cpy operand: %u\n", operand);
    setOrClearStatusIf(mY >= operand, ProcessorStatus::Carry);
    setOrClearStatusIf(mY == operand, ProcessorStatus::Zero);
    setOrClearStatusIf((mY - operand) & 0b10000000, ProcessorStatus::Negative);
    mPC++;
}

void CPU::SBC(MemoryAccessMode mode)
{
    u8 operand = getOperand(mode);
    ADCImpl(~operand);
    mPC++;
}


void CPU::DEC(MemoryAccessMode mode)
{
    u16 address = getAddressOperand(mode);

    auto& bus = Bus::the();
    auto value = bus.readMemory(address);
    bus.writeMemory(address, 1 + value);
    mPC++;
}
void CPU::INC(MemoryAccessMode mode)
{
    u16 address = getAddressOperand(mode);
    
    auto& bus = Bus::the();
    auto value = bus.readMemory(address);
    bus.writeMemory(address, 1 + value);
    mPC++;
}

void CPU::DEX(MemoryAccessMode)
{
    mX--;
    setOrClearStatusIf(mX == 0, ProcessorStatus::Zero);
    setOrClearStatusIf(mX & 0b10000000, ProcessorStatus::Negative);

    mPC++;
}
void CPU::DEY(MemoryAccessMode)
{
    mY--;
    setOrClearStatusIf(mY == 0, ProcessorStatus::Zero);
    setOrClearStatusIf(mY & 0b10000000, ProcessorStatus::Negative);
    mPC++;
}
void CPU::INX(MemoryAccessMode)
{
    mX++;
    setOrClearStatusIf(mX == 0, ProcessorStatus::Zero);
    setOrClearStatusIf(mX & 0b10000000, ProcessorStatus::Negative);
    mPC++;
}
void CPU::INY(MemoryAccessMode)
{
    mY++;
    setOrClearStatusIf(mY == 0, ProcessorStatus::Zero);
    setOrClearStatusIf(mY & 0b10000000, ProcessorStatus::Negative);

    mPC++;
}

void CPU::JMP(MemoryAccessMode mode)
{
    auto& bus = Bus::the();
    switch (mode) {
    case MemoryAccessMode::Absolute: {
	mPC = bus.readMemory16Bits(mPC + 1);
	break;
    }
    case MemoryAccessMode::Indirect: {
	printf("The argument is: %08x\n", bus.readMemory16Bits(mPC + 1));
	printf("The value from the argument is: %08x\n", bus.readMemory16Bits(bus.readMemory16Bits(mPC + 1)));
	mPC = bus.readMemory16Bits(bus.readMemory16Bits(mPC + 1));
	exit(1);
	break;
    }
    default:
	assert(false);
    }
}


void CPU::CLV(MemoryAccessMode)
{
    clearProcessorStatus(ProcessorStatus::Overflow);
    mPC++;
}

void CPU::CLC(MemoryAccessMode)
{
    clearProcessorStatus(ProcessorStatus::Carry);
    mPC++;
}
void CPU::CLD(MemoryAccessMode)
{
    clearProcessorStatus(ProcessorStatus::DecimalMode);
    mPC++;
}

void CPU::RTS(MemoryAccessMode)
{
    mPC = popWord();
    printf("I happen and pop to %u\n", mPC);
}

void CPU::LDX(MemoryAccessMode mode)
{
    u8 operand = getOperand(mode);
    mX = operand;
    setOrClearStatusIf(mX == 0, ProcessorStatus::Zero);
    setOrClearStatusIf(mX & 0b10000000, ProcessorStatus::Negative);
    mPC++;
}
void CPU::LDY(MemoryAccessMode mode)
{
    u8 operand = getOperand(mode);
    mY = operand;
    setOrClearStatusIf(mY == 0, ProcessorStatus::Zero);
    setOrClearStatusIf(mY & 0b10000000, ProcessorStatus::Negative);
    mPC++;
}

void CPU::JSR(MemoryAccessMode)
{
    auto destination = Bus::the().readMemory16Bits(++mPC);
    mPC += 2;
    pushWord(mPC);
    mPC = destination;
}

void CPU::NOP(MemoryAccessMode)
{
    mPC++;
    mClockCycle += 2;
    if (u8 result = Bus::the().readMemory(0); result > 0) {
	printf("!!!!! FAILED TEST, error code %x !!!!!\n", result);
	exit(1);
    }
    static unsigned testNumber = 0;
    printf("----- Starting test %x -----\n", ++testNumber);
}

void CPU::SEC(MemoryAccessMode)
{
    setProcessorStatus(ProcessorStatus::Carry);
    mPC++;
    mClockCycle += 2;
}
void CPU::SEI(MemoryAccessMode)
{
    setProcessorStatus(ProcessorStatus::InterruptDisable);
    mPC++;
    mClockCycle += 2;
}
void CPU::SED(MemoryAccessMode)
{
    setProcessorStatus(ProcessorStatus::DecimalMode);
    mPC++;
    mClockCycle += 2;
}

void CPU::PHP(MemoryAccessMode)
{
    pushByte((u8)mP);
    printf("Status being pushed: %u\n", mP);
    mPC++;
    mClockCycle += 2;
}
void CPU::PLA(MemoryAccessMode)
{
    mA = popByte();
    printf("Status being popped: %08x\n", mA);
    setOrClearStatusIf(mA == 0, ProcessorStatus::Zero);
    setOrClearStatusIf(mA & 0b10000000, ProcessorStatus::Negative);

    mPC++;
    mClockCycle += 2;
}
void CPU::PLP(MemoryAccessMode)
{
    mP = (ProcessorStatus)popByte();
    printf("Status being popped: %08x\n", mA);

    mPC++;
    mClockCycle += 2;
}

void CPU::BCS(MemoryAccessMode)
{
    mPC++;
    if (processorStatusHas(ProcessorStatus::Carry)) {
	mPC += decode8Bits();
	return;
    }
    mPC++;
}

void CPU::BCC(MemoryAccessMode)
{
    mPC++;
    if (!processorStatusHas(ProcessorStatus::Carry)) {
	mPC += decode8Bits();
	return;
    }
    mPC++;
}

void CPU::BEQ(MemoryAccessMode)
{
    mPC++;
    if (processorStatusHas(ProcessorStatus::Zero)) {
	mPC += decode8Bits();
	return;
    }
    mPC++;
}

void CPU::BNE(MemoryAccessMode)
{
    mPC++;
    if (!processorStatusHas(ProcessorStatus::Zero)) {
	printf("BNE Branches!\n");
	mPC += decode8Bits();
	return;
    }
    mPC++;
}
void CPU::BVS(MemoryAccessMode)
{
    mPC++;
    mClockCycle += 2;
    if (processorStatusHas(ProcessorStatus::Overflow)) {
	printf("BVS Branches!\n");
	mPC += decode8Bits();
	mClockCycle += 1;
	return;
    }
    mPC++;
}

void CPU::BVC(MemoryAccessMode)
{
    mPC++;
    mClockCycle += 2;
    if (!processorStatusHas(ProcessorStatus::Overflow)) {
	printf("BVC Branches!\n");
	mPC += decode8Bits();
	mClockCycle += 1;
	return;
    }
    mPC++;
}
void CPU::BMI(MemoryAccessMode)
{
    mPC++;
    mClockCycle += 2;
    if (processorStatusHas(ProcessorStatus::Negative)) {
	printf("BMI Branches!\n");
	mPC += decode8Bits();
	mClockCycle += 1;
	return;
    }
    mPC++;
}
void CPU::BPL(MemoryAccessMode)
{
    mPC++;
    mClockCycle += 2;
    if (!processorStatusHas(ProcessorStatus::Negative)) {
	printf("BPL Branches!\n");
	mPC += decode8Bits();
	mClockCycle += 1;
	return;
    }
    mPC++;
}

void CPU::STA(MemoryAccessMode mode)
{
    auto addr = getAddressOperand(mode);
    Bus::the().writeMemory(addr, mA);
    mPC++;
    normallyIncrementClockCycle(mode);
}
void CPU::STX(MemoryAccessMode mode)
{
    auto addr = getAddressOperand(mode);
    Bus::the().writeMemory(addr, mX);
    mPC++;
    normallyIncrementClockCycle(mode);
}
void CPU::STY(MemoryAccessMode mode)
{
    auto addr = getAddressOperand(mode);
    Bus::the().writeMemory(addr, mY);
    mPC++;
    normallyIncrementClockCycle(mode);
}



void CPU::BIT(MemoryAccessMode mode)
{
    auto operand = getOperand(mode);
    printf("BIT Operand: %u, Accumulator: %u\n", operand, mA);
    auto result = mA & operand;
    printf("The result of the BIT operation is: %u\n", result);
    setOrClearStatusIf(!result, ProcessorStatus::Zero);
    setOrClearStatusIf(operand & 0b10000000, ProcessorStatus::Negative);
    setOrClearStatusIf(operand & 0b01000000, ProcessorStatus::Overflow);

    mPC++;
    normallyIncrementClockCycle(mode);
}

void CPU::TAY(MemoryAccessMode)
{
    mY = mA;
    setOrClearStatusIf(mY == 0, ProcessorStatus::Zero);
    setOrClearStatusIf(mY & 0b10000000, ProcessorStatus::Negative);

    mPC++;
    mClockCycle += 2;
}
void CPU::TAX(MemoryAccessMode)
{
    mX = mA;
    setOrClearStatusIf(mX == 0, ProcessorStatus::Zero);
    setOrClearStatusIf(mX & 0b10000000, ProcessorStatus::Negative);

    mPC++;
    mClockCycle += 2;
}
void CPU::TXA(MemoryAccessMode)
{
    mA = mX;
    setOrClearStatusIf(mX == 0, ProcessorStatus::Zero);
    setOrClearStatusIf(mX & 0b10000000, ProcessorStatus::Negative);

    mPC++;
    mClockCycle += 2;
}
void CPU::TYA(MemoryAccessMode)
{
    mA = mY;
    setOrClearStatusIf(mY == 0, ProcessorStatus::Zero);
    setOrClearStatusIf(mY & 0b10000000, ProcessorStatus::Negative);

    mPC++;
    mClockCycle += 2;
}
void CPU::TSX(MemoryAccessMode)
{
    mX = mSP;
    setOrClearStatusIf(mX == 0, ProcessorStatus::Zero);
    setOrClearStatusIf(mX & 0b10000000, ProcessorStatus::Negative);

    mPC++;
    mClockCycle += 2;
}
void CPU::TXS(MemoryAccessMode)
{
    mSP = mX;

    mPC++;
    mClockCycle += 2;
}



}
