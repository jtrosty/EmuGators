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

void CPU::printStack(u8 entries)
{
    printf("Stack Pointer: %08x\n", mSP + 0x100);
    printf("Stack output:\n");
    for (u8 i = 0; i < entries; i++) {
	printf("%08x\t%08x\n", 0x1ff - i, Bus::the().readMemory(0x1ff - i));
    }
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

void CPU::pushByte(byte b)
{
    fprintf(stderr, "Stack pointer before 1 byte push: %08x\n", mSP);
    Bus::the().writeMemory((u16)mSP + 0x100, b);
    mSP--;
}
u8 CPU::popByte()
{
    fprintf(stderr, "Stack pointer before 1 byte pop: %08x\n", mSP);
    return Bus::the().readMemory((u16)(++mSP) + 0x100);
}
u8 CPU::peekByte()
{
    return Bus::the().readMemory((u16)(mSP + 1) + 0x100);
}

void CPU::pushWord(u16 word)
{
    fprintf(stderr, "Stack pointer before 2 byte push: %08x\n", mSP);
    pushByte(word >> 8);
    pushByte(word & 0xff);
}
u16 CPU::popWord()
{
    u16 value = popByte();
    return value | ((u16)popByte() << 8);
}
u16 CPU::peekWord()
{
    return Bus::the().readMemory16Bits((u16)mSP + 0x100);
}



void CPU::runOpcode(EncodedInstructionType inst)
{
    switch (inst) {
	IS_OPCODE(BRK, 0x0, Implicit);
	IS_OPCODE(JMP, 0x4c, Absolute);
	IS_OPCODE(JMP, 0x6c, Indirect);
	IS_OPCODE(CLV, 0xb8, Implicit);
	IS_OPCODE(CLC, 0x18, Implicit);
	IS_OPCODE(CLD, 0xd8, Implicit);
	IS_OPCODE(CLI, 0x58, Implicit);
	IS_OPCODE(RTS, 0x60, Implicit);
	IS_OPCODE(RTI, 0x40, Implicit);
	IS_OPCODE(LDY, 0xa0, Immediate);
	IS_OPCODE(LDY, 0xa4, ZeroPage);
	IS_OPCODE(LDY, 0xb4, XZeroPageIndexed);
	IS_OPCODE(LDY, 0xac, Absolute);
	IS_OPCODE(LDY, 0xbc, XAbsoluteIndexed);
	IS_OPCODE(LDX, 0xa2, Immediate);
	IS_OPCODE(LDX, 0xa6, ZeroPage);
	IS_OPCODE(LDX, 0xb6, YZeroPageIndexed);
	IS_OPCODE(LDX, 0xae, Absolute);
	IS_OPCODE(LDX, 0xbe, YAbsoluteIndexed);
	IS_OPCODE(STY, 0x84, ZeroPage);
	IS_OPCODE(STY, 0x94, XZeroPageIndexed);
	IS_OPCODE(STY, 0x8c, Absolute);
	IS_OPCODE(STX, 0x86, ZeroPage);
	IS_OPCODE(STX, 0x96, YZeroPageIndexed);
	IS_OPCODE(STX, 0x8e, Absolute);
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
	IS_OPCODE(CPY, 0xc4, ZeroPage);
	IS_OPCODE(CPY, 0xcc, Absolute);
	IS_OPCODE(CPX, 0xe0, Immediate);
	IS_OPCODE(CPX, 0xe4, ZeroPage);
	IS_OPCODE(CPX, 0xec, Absolute);
	IS_OPCODE(INC, 0xe6, ZeroPage);
	IS_OPCODE(INC, 0xf6, XZeroPageIndexed);
	IS_OPCODE(INC, 0xee, Absolute);
	IS_OPCODE(INC, 0xfe, XAbsoluteIndexed);
	IS_OPCODE(INY, 0xc8, Implicit);
	IS_OPCODE(INX, 0xe8, Implicit);
	IS_OPCODE(DEC, 0xc6, ZeroPage);
	IS_OPCODE(DEC, 0xd6, XZeroPageIndexed);
	IS_OPCODE(DEC, 0xce, Absolute);
	IS_OPCODE(DEC, 0xde, XAbsoluteIndexed);
	IS_OPCODE(DEY, 0x88, Implicit);
	IS_OPCODE(DEX, 0xca, Implicit);
	IS_OPCODE(TAX, 0xaa, Implicit);
	IS_OPCODE(TAY, 0xa8, Implicit);
	IS_OPCODE(TSX, 0xba, Implicit);
	IS_OPCODE(TXA, 0x8a, Implicit);
	IS_OPCODE(TXS, 0x9a, Implicit);
	IS_OPCODE(TYA, 0x98, Implicit);
	IS_OPCODE(LSR, 0x4a, Accumulator);
	IS_OPCODE(LSR, 0x46, ZeroPage);
	IS_OPCODE(LSR, 0x56, XZeroPageIndexed);
	IS_OPCODE(LSR, 0x4e, Absolute);
	IS_OPCODE(LSR, 0x5e, XAbsoluteIndexed);
	IS_OPCODE(ASL, 0x0a, Accumulator);
	IS_OPCODE(ASL, 0x06, ZeroPage);
	IS_OPCODE(ASL, 0x16, XZeroPageIndexed);
	IS_OPCODE(ASL, 0x0e, Absolute);
	IS_OPCODE(ASL, 0x1e, XAbsoluteIndexed);
	IS_OPCODE(ROR, 0x6a, Accumulator);
	IS_OPCODE(ROR, 0x66, ZeroPage);
	IS_OPCODE(ROR, 0x76, XZeroPageIndexed);
	IS_OPCODE(ROR, 0x6e, Absolute);
	IS_OPCODE(ROR, 0x7e, XAbsoluteIndexed);
	IS_OPCODE(ROL, 0x2a, Accumulator);
	IS_OPCODE(ROL, 0x26, ZeroPage);
	IS_OPCODE(ROL, 0x36, XZeroPageIndexed);
	IS_OPCODE(ROL, 0x2e, Absolute);
	IS_OPCODE(ROL, 0x3e, XAbsoluteIndexed);
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
	EncodedInstructionType opcode = bus.rawMemory()[mPC++];
	printf("Program counter: %08x, Instruction: %08x\n", mPC - 1, opcode);
	runOpcode(opcode);
	if (u8 result = Bus::the().readMemory(2); result > 0) {
	    printf("Error code: %08x\n", result);
	    exit(1);
	}
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
    case MemoryAccessMode::Immediate: {
	u8 val = bus.readMemory(mPC);
	mPC++;
	return val;
    }
    case MemoryAccessMode::ZeroPage: {
	u8 val = bus.readMemory(bus.readMemory(mPC));
	++mPC;
	return val;
    }
    case MemoryAccessMode::Absolute: {
	u8 value = bus.readMemory(bus.readMemory16Bits(mPC));
	//printf("Program counter before increment: %08x\n", mPC);
	mPC += 2;
	//printf("Program counter after increment: %08x\n", mPC);
	return value;
    }
    case MemoryAccessMode::XAbsoluteIndexed: {
	u8 value = bus.readMemory(bus.readMemory16Bits(mPC) + mX);
	mPC += 2;
	return value;
    }
    case MemoryAccessMode::YAbsoluteIndexed: {
	u8 value = bus.readMemory(bus.readMemory16Bits(mPC) + mY);
	mPC += 2;
	return value;
    }
    case MemoryAccessMode::XZeroPageIndexed: {
	u8 val = bus.readMemory((bus.readMemory(mPC) + mX) % 256);
	mPC++;
	return val;
    }
    case MemoryAccessMode::YZeroPageIndexed: {
	u8 val = bus.readMemory((bus.readMemory(mPC) + mY) % 256);
	mPC++;
	return val;
    }
    case MemoryAccessMode::XIndirectIndexed: {
	// Not normal, just leftovers from debugging a silly issue. Lazy me.
	u16 arg = bus.readMemory(mPC);
	u16 leftHalf = (u16)bus.readMemory((arg + mX) % 256);
	u16 rightHalfBeforeRead = (arg + 1 + mX) % 256;
	u16 rightHalf = (u16)bus.readMemory(rightHalfBeforeRead) * 256;
	u16 address = leftHalf + rightHalf;

	u8 val = bus.readMemory(address);
	mPC += 1;
	return val;
    }
    case MemoryAccessMode::YIndirectIndexed: {
	u16 arg = bus.readMemory(mPC);
	u16 address = bus.readMemory(arg) + bus.readMemory((arg + 1) % 256) * 256 + mY;
	u8 val = bus.readMemory(address);
	mPC += 1;
	return val;
    }
    default:
	fprintf(stderr, "Unimplemented memory access mode number: %u!\n", (unsigned)mode);
	exit(1);
    }
}

u16 CPU::getAddressOperand(MemoryAccessMode mode)
{
    auto& bus = Bus::the();
    switch (mode) {
    case MemoryAccessMode::Accumulator:
	return mA;
    case MemoryAccessMode::Immediate:
    case MemoryAccessMode::ZeroPage: {
	 u16 val = bus.readMemory(mPC);
	 mPC++;
	 return val;
    }
    case MemoryAccessMode::Absolute: {
	u16 address = bus.readMemory16Bits(mPC);
	mPC += 2;
	return address;
    }
    case MemoryAccessMode::XAbsoluteIndexed: {
	u16 address = bus.readMemory16Bits(mPC) + mX;
	mPC += 2;
	return address;
    }
    case MemoryAccessMode::YAbsoluteIndexed: {
	u16 address = bus.readMemory16Bits(mPC) + mY;
	mPC += 2;
	return address;
    }
    case MemoryAccessMode::XZeroPageIndexed: {
	u16 addr = (bus.readMemory(mPC) + mX) % 256;
	mPC++;
	return addr;
    }
    case MemoryAccessMode::YZeroPageIndexed: {
	u16 addr = (bus.readMemory(mPC) + mY) % 256;
	mPC++;
	return addr;
    }
    case MemoryAccessMode::XIndirectIndexed: {
	//exit(1);
	u16 arg = bus.readMemory(mPC);
	u16 address = (u16)bus.readMemory((arg + mX) % 256) + (u16)bus.readMemory((arg + 1 + mX) % 256) * 256;
	//printf("------ The XIndirectIndexed address read is %08x ------\n", address);
	mPC += 1;
	return address;
    }
    case MemoryAccessMode::YIndirectIndexed: {
	u16 arg = bus.readMemory(mPC);
	u16 address = bus.readMemory(arg) + bus.readMemory((arg + 1) % 256) * 256 + mY;
	mPC += 1;
	return address;
    }

    default:
       fprintf(stderr, "getAddressOperand does not have that access mode implemented yet!\n");
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
    u16 val = Bus::the().readMemory16Bits(mPC);
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
    // How should BRK quit the system if at all?
    exit(1);
}

void CPU::PHA(MemoryAccessMode)
{
    pushByte(mA);
    mClockCycle += 3;
}

void CPU::ORA(MemoryAccessMode mode)
{
    printf("ORA\n");
    u8 operand = getOperand(mode);
    mA |= operand;
    setOrClearStatusIf(mA == 0, ProcessorStatus::Zero);
    setOrClearStatusIf(mA & 0b10000000, ProcessorStatus::Negative);
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
}

void CPU::EOR(MemoryAccessMode mode)
{
    // just accumulator mode
    u8 operand = getOperand(mode);
    mA ^= operand;
    setOrClearStatusIf(mA == 0, ProcessorStatus::Zero);
    setOrClearStatusIf(mA & 0b10000000, ProcessorStatus::Negative);
}


void CPU::leftShiftImpl(MemoryAccessMode mode, ShiftType type)
{
    u8 value;
    u16 address;
    if (mode == MemoryAccessMode::Accumulator) {
	value = mA;
    } else {
	address = getAddressOperand(mode);
	value = Bus::the().readMemory(address);
    }
    bool oldCarry = processorStatusHas(ProcessorStatus::Carry);
    setOrClearStatusIf(value & 0b10000000, ProcessorStatus::Carry);
    value <<= 1;
    if (type == ShiftType::Rotate) {
	value |= (unsigned)oldCarry;
    }

    setOrClearStatusIf(value == 0, ProcessorStatus::Zero);
    setOrClearStatusIf(value & 0b10000000, ProcessorStatus::Negative);

    if (mode == MemoryAccessMode::Accumulator)
	mA = value;
    else
	Bus::the().writeMemory(address, value);
}
void CPU::rightShiftImpl(MemoryAccessMode mode, ShiftType type)
{
    u8 value;
    u16 address;
    if (mode == MemoryAccessMode::Accumulator) {
	value = mA;
    } else {
	address = getAddressOperand(mode);
	value = Bus::the().readMemory(address);
    }
    bool oldCarry = processorStatusHas(ProcessorStatus::Carry);
    setOrClearStatusIf(value & 0b1, ProcessorStatus::Carry);
    value >>= 1;
    if (type == ShiftType::Rotate) {
	value |= oldCarry ? 0b10000000 : 0b0;
    }
    setOrClearStatusIf(value == 0, ProcessorStatus::Zero);
    setOrClearStatusIf(value & 0x80, ProcessorStatus::Negative);

    if (mode == MemoryAccessMode::Accumulator)
	mA = value;
    else
	Bus::the().writeMemory(address, value);
}

// These don't work right! And is it normal for accumulator mode to be like that in other memory things? (or some question like that)
void CPU::LSR(MemoryAccessMode mode)
{
    rightShiftImpl(mode, ShiftType::Arithmetic);
    //normallyIncrementClockCycle(mode, 2);
}
void CPU::ASL(MemoryAccessMode mode)
{
    leftShiftImpl(mode, ShiftType::Arithmetic);
    //normallyIncrementClockCycle(mode, 2);
}

void CPU::ROR(MemoryAccessMode mode)
{
    rightShiftImpl(mode, ShiftType::Rotate);
    //setOrClearStatusIf(processorStatusHas(ProcessorStatus::Carry), ProcessorStatus::Negative);
    
}
void CPU::ROL(MemoryAccessMode mode)
{
    leftShiftImpl(mode, ShiftType::Rotate);
}


void CPU::ADC(MemoryAccessMode mode)
{
    u8 operand = getOperand(mode);
    ADCImpl(operand);
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
    //normallyIncrementClockCycle(mode);
}

void CPU::CMP(MemoryAccessMode mode)
{
    // just accumulator mode
    u8 operand = getOperand(mode);
    printf("value of cmp operand: %08x, Accumulator: %08x\n", operand, mA);
    setOrClearStatusIf(mA >= operand, ProcessorStatus::Carry);
    setOrClearStatusIf(mA == operand, ProcessorStatus::Zero);
    setOrClearStatusIf((mA - operand) & 0b10000000, ProcessorStatus::Negative);
}
void CPU::CPX(MemoryAccessMode mode)
{
    u8 operand = getOperand(mode);
    printf("value of CPX operand: %u\n", operand);
    setOrClearStatusIf(mX >= operand, ProcessorStatus::Carry);
    setOrClearStatusIf(mX == operand, ProcessorStatus::Zero);
    setOrClearStatusIf((mX - operand) & 0b10000000, ProcessorStatus::Negative);
}
void CPU::CPY(MemoryAccessMode mode)
{
    u8 operand = getOperand(mode);
    printf("value of cpy operand: %08x, with mode number %u\n", operand, mode);
    setOrClearStatusIf(mY >= operand, ProcessorStatus::Carry);
    setOrClearStatusIf(mY == operand, ProcessorStatus::Zero);
    setOrClearStatusIf((mY - operand) & 0b10000000, ProcessorStatus::Negative);
}

void CPU::SBC(MemoryAccessMode mode)
{
    u8 operand = getOperand(mode);
    ADCImpl(~operand);
}


void CPU::DEC(MemoryAccessMode mode)
{
    u16 address = getAddressOperand(mode);

    auto& bus = Bus::the();
    u8 value = bus.readMemory(address) - 1;
    bus.writeMemory(address, value);
    setOrClearStatusIf(value == 0, ProcessorStatus::Zero);
    setOrClearStatusIf(value & 0b10000000, ProcessorStatus::Negative);
}
void CPU::INC(MemoryAccessMode mode)
{
    u16 address = getAddressOperand(mode);
    
    auto& bus = Bus::the();
    u8 value = bus.readMemory(address) + 1;
    bus.writeMemory(address, value);
    setOrClearStatusIf(value == 0, ProcessorStatus::Zero);
    setOrClearStatusIf(value & 0b10000000, ProcessorStatus::Negative);
}

void CPU::DEX(MemoryAccessMode)
{
    mX--;
    setOrClearStatusIf(mX == 0, ProcessorStatus::Zero);
    setOrClearStatusIf(mX & 0b10000000, ProcessorStatus::Negative);
    mClockCycle += 2;
}
void CPU::DEY(MemoryAccessMode)
{
    mY--;
    setOrClearStatusIf(mY == 0, ProcessorStatus::Zero);
    setOrClearStatusIf(mY & 0b10000000, ProcessorStatus::Negative);
    mClockCycle += 2;
}
void CPU::INX(MemoryAccessMode)
{
    mX++;
    setOrClearStatusIf(mX == 0, ProcessorStatus::Zero);
    setOrClearStatusIf(mX & 0b10000000, ProcessorStatus::Negative);
    mClockCycle += 2;
}
void CPU::INY(MemoryAccessMode)
{
    mY++;
    setOrClearStatusIf(mY == 0, ProcessorStatus::Zero);
    setOrClearStatusIf(mY & 0b10000000, ProcessorStatus::Negative);
    mClockCycle += 2;
}

void CPU::JMP(MemoryAccessMode mode)
{
    auto& bus = Bus::the();
    switch (mode) {
    case MemoryAccessMode::Absolute: {
	mClockCycle += 3;
	mPC = bus.readMemory16Bits(mPC);
	break;
    }
    case MemoryAccessMode::Indirect: {
	mClockCycle += 5;
	printf("The argument is: %08x\n", bus.readMemory16Bits(mPC));
	u16 address = bus.readMemory16Bits(mPC);
	if ((address & 0xff) == 0xff) {
	    mPC = (u16)bus.readMemory(address) + (u16((bus.readMemory(address & 0xff00)) << 8));
	    break;
	}
	mPC = bus.readMemory16Bits(address);
	printf("The value from the argument is: %08x\n", mPC);
	break;
    }
    default:
	assert(false);
    }
}


void CPU::CLV(MemoryAccessMode)
{
    clearProcessorStatus(ProcessorStatus::Overflow);
    mClockCycle += 2;
}

void CPU::CLC(MemoryAccessMode)
{
    clearProcessorStatus(ProcessorStatus::Carry);
    mClockCycle += 2;
}
void CPU::CLD(MemoryAccessMode)
{
    clearProcessorStatus(ProcessorStatus::DecimalMode);
    mClockCycle += 2;
}
void CPU::CLI(MemoryAccessMode)
{
    clearProcessorStatus(ProcessorStatus::InterruptDisable);
    mClockCycle += 2;
}

void CPU::RTS(MemoryAccessMode)
{
    printf("RTS: Before stack popping\n");
    printStack(10);
    mPC = popWord() + 1;
#if 1
    if (u8 result = Bus::the().readMemory(0); result > 0) {
	printf("!!!!! FAILED TEST location 00h, error code %08x !!!!!\n", result);
	exit(1);
    }
#endif
    if (u8 result = Bus::the().readMemory(2); result > 0) {
	printf("!!!!! FAILED TEST location 02h, error code %08x !!!!!\n", result);
	exit(1);
    }
    if (u8 result = Bus::the().readMemory(3); result > 0) {
	printf("!!!!! FAILED TEST location 03h, error code %08x !!!!!\n", result);
	exit(1);
    }

    printf("I happen and pop to %08x\n", mPC);
}
void CPU::RTI(MemoryAccessMode)
{
    mP = (ProcessorStatus)popByte();
    mPC = popWord();
    printf("RTI happens and I pop to %08x\n", mPC);
    //exit(1);
}

void CPU::LDX(MemoryAccessMode mode)
{
    u8 operand = getOperand(mode);
    mX = operand;
    printf("LDX turns mX into %08x\n", mX);
    setOrClearStatusIf(mX == 0, ProcessorStatus::Zero);
    setOrClearStatusIf(mX & 0b10000000, ProcessorStatus::Negative);
}
void CPU::LDY(MemoryAccessMode mode)
{
    u8 operand = getOperand(mode);
    mY = operand;
    setOrClearStatusIf(mY == 0, ProcessorStatus::Zero);
    setOrClearStatusIf(mY & 0b10000000, ProcessorStatus::Negative);
}

void CPU::JSR(MemoryAccessMode)
{
    pushWord(mPC + 1);
    printStack(10);
    u16 destination = Bus::the().readMemory16Bits(mPC);
    mPC = destination;
    mClockCycle += 6;
}

void CPU::NOP(MemoryAccessMode)
{
    mClockCycle += 2;
    static unsigned testNumber = 0;
    if (u8 result = Bus::the().readMemory(2); result != 0) {
	printf("!!!!! FAILED TEST location 02h, error code %08x !!!!!\n", result);
	exit(1);
    }
    if (u8 result = Bus::the().readMemory(3); result != 0) {
	printf("!!!!! FAILED TEST location 03h, error code %08x !!!!!\n", result);
	exit(1);
    }
    printf("----- Starting test %x -----\n", ++testNumber);
}

void CPU::SEC(MemoryAccessMode)
{
    setProcessorStatus(ProcessorStatus::Carry);
    mClockCycle += 2;
}
void CPU::SEI(MemoryAccessMode)
{
    setProcessorStatus(ProcessorStatus::InterruptDisable);
    mClockCycle += 2;
}
void CPU::SED(MemoryAccessMode)
{
    setProcessorStatus(ProcessorStatus::DecimalMode);
    mClockCycle += 2;
}

void CPU::PHP(MemoryAccessMode)
{
    pushByte((u8)mP);
    printf("Status being pushed: %u\n", mP);
    mClockCycle += 2;
}
void CPU::PLA(MemoryAccessMode)
{
    mA = popByte();
    printf("Status being popped: %08x\n", mA);
    setOrClearStatusIf(mA == 0, ProcessorStatus::Zero);
    setOrClearStatusIf(mA & 0b10000000, ProcessorStatus::Negative);

    mClockCycle += 2;
}
void CPU::PLP(MemoryAccessMode)
{
    mP = (ProcessorStatus)popByte();
    printf("Status being popped: %08x\n", mA);

    mClockCycle += 2;
}

void CPU::branchImpl(bool condition)
{
    mClockCycle += 2;
    if (condition) {
	mPC += decode8Bits();

	return;
    }
    mPC++;
}

void CPU::BCS(MemoryAccessMode)
{
    branchImpl(processorStatusHas(ProcessorStatus::Carry));
}

void CPU::BCC(MemoryAccessMode)
{
    branchImpl(!processorStatusHas(ProcessorStatus::Carry));
}

void CPU::BEQ(MemoryAccessMode)
{
    branchImpl(processorStatusHas(ProcessorStatus::Zero));
}

void CPU::BNE(MemoryAccessMode)
{
    branchImpl(!processorStatusHas(ProcessorStatus::Zero));
}
void CPU::BVS(MemoryAccessMode)
{
    branchImpl(processorStatusHas(ProcessorStatus::Overflow));
}

void CPU::BVC(MemoryAccessMode)
{
    branchImpl(!processorStatusHas(ProcessorStatus::Overflow));
}
void CPU::BMI(MemoryAccessMode)
{
    branchImpl(processorStatusHas(ProcessorStatus::Negative));
}
void CPU::BPL(MemoryAccessMode)
{
    branchImpl(!processorStatusHas(ProcessorStatus::Negative));
}

void CPU::STA(MemoryAccessMode mode)
{
    u16 addr = getAddressOperand(mode);
    printf("The address that STA got: %08x\n", addr);
    Bus::the().writeMemory(addr, mA);
    //normallyIncrementClockCycle(mode);
}
void CPU::STX(MemoryAccessMode mode)
{
    u16 addr = getAddressOperand(mode);
    Bus::the().writeMemory(addr, mX);
    printf("STX stores value %08x at %08x\n", mX, addr);
    //normallyIncrementClockCycle(mode);
}
void CPU::STY(MemoryAccessMode mode)
{
    u16 addr = getAddressOperand(mode);
    Bus::the().writeMemory(addr, mY);
    //normallyIncrementClockCycle(mode);
}



void CPU::BIT(MemoryAccessMode mode)
{
    u8 operand = getOperand(mode);
    printf("BIT Operand: %u, Accumulator: %u\n", operand, mA);
    u8 result = mA & operand;
    printf("The result of the BIT operation is: %u\n", result);
    setOrClearStatusIf(!result, ProcessorStatus::Zero);
    setOrClearStatusIf(operand & 0b10000000, ProcessorStatus::Negative);
    setOrClearStatusIf(operand & 0b01000000, ProcessorStatus::Overflow);

    //normallyIncrementClockCycle(mode);
}

void CPU::TAY(MemoryAccessMode)
{
    mY = mA;
    setOrClearStatusIf(mY == 0, ProcessorStatus::Zero);
    setOrClearStatusIf(mY & 0b10000000, ProcessorStatus::Negative);

    mClockCycle += 2;
}
void CPU::TAX(MemoryAccessMode)
{
    mX = mA;
    setOrClearStatusIf(mX == 0, ProcessorStatus::Zero);
    setOrClearStatusIf(mX & 0b10000000, ProcessorStatus::Negative);

    mClockCycle += 2;
}
void CPU::TXA(MemoryAccessMode)
{
    mA = mX;
    setOrClearStatusIf(mX == 0, ProcessorStatus::Zero);
    setOrClearStatusIf(mX & 0b10000000, ProcessorStatus::Negative);

    mClockCycle += 2;
}
void CPU::TYA(MemoryAccessMode)
{
    mA = mY;
    setOrClearStatusIf(mY == 0, ProcessorStatus::Zero);
    setOrClearStatusIf(mY & 0b10000000, ProcessorStatus::Negative);

    mClockCycle += 2;
}
void CPU::TSX(MemoryAccessMode)
{
    mX = mSP;
    setOrClearStatusIf(mX == 0, ProcessorStatus::Zero);
    setOrClearStatusIf(mX & 0b10000000, ProcessorStatus::Negative);

    mClockCycle += 2;
}
void CPU::TXS(MemoryAccessMode)
{
    mSP = mX;
    mClockCycle += 2;
}

}
