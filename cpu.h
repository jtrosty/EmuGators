#pragma once

#include "assert.h"
#include "defs.h"
#include "forward.h"
#include "badge.h"
#include "device.h"
#include <stdio.h>
#include "bus.h"


#define UNDEFINED_INSTRUCTION(instruction) \
    fprintf(stderr, "Instruction %08x not found\n", instruction);	\
    assert(false);


#define IS_ALU_OPCODE(opcode, offset, memoryAccessMode)		\
    case (unsigned)ALUInstructionOpcodeBase::opcode + offset:	\
    opcode(MemoryAccessMode::memoryAccessMode);	    \
    break

#define IS_OPCODE(instruction, opcode, memoryAccessMode) \
    case opcode: \
    instruction(MemoryAccessMode::memoryAccessMode);	\
    break


#define RUN_IF_ALU_OPCODE(opcode) \
    IS_ALU_OPCODE(opcode, 0x01, XIndirectIndexed);	\
    IS_ALU_OPCODE(opcode, 0x05, ZeroPage);		\
    IS_ALU_OPCODE(opcode, 0x09, Immediate);		\
    IS_ALU_OPCODE(opcode, 0x0D, Absolute);		\
    IS_ALU_OPCODE(opcode, 0x11, YIndirectIndexed);	\
    IS_ALU_OPCODE(opcode, 0x15, XZeroPageIndexed);	\
    IS_ALU_OPCODE(opcode, 0x19, YAbsoluteIndexed);	\
    IS_ALU_OPCODE(opcode, 0x1D, XAbsoluteIndexed)


namespace NESEmulator {

class CPU : public Device<CPU> {
public:
    void reset();	/* Can the contents of this just be in the constructor? Is reset ever run outside of powering on the machine? Does it ever truly reboot? */

    using EncodedInstructionType = u8;

    enum class ALUInstructionOpcodeBase {
	ORA = 0x0,
	AND = 0x20,
	EOR = 0x40,
	ADC = 0x60,
	STA = 0x80,
	LDA = 0xA0,
	CMP = 0xC0,
	SBC = 0xE0,
    };

    
    enum class MemoryAccessMode {
	Implicit = 0, 		/* Imp */
	Accumulator,		/* A */
	Immediate,		/* #v */
	ZeroPage,		/* d */
	Absolute,		/* a */
	Relative,		/* label */
	Indirect,		/* (a) */
	XZeroPageIndexed,	/* d, x */
	YZeroPageIndexed,       /* d, y */
	XAbsoluteIndexed,	/* a, x */
	YAbsoluteIndexed,       /* a, y */
	XIndirectIndexed,       /* (d, x) */
	YIndirectIndexed,       /* (d), y */
    };
    
    enum class ProcessorStatus {
	Carry = 1 << 0,
	Zero = 1 << 1,
	InterruptDisable = 1 << 2,
	DecimalMode = 1 << 3,
	BreakCommand = 1 << 4,
	Overflow = 1 << 6,
	Negative = 1 << 7,
    };
    
    void printStack(u8 entries);

    void execLoop();
    void step(u32 = 1);
    void pushByte(byte b);
    u8 popByte();
    u8 peekByte();

    void pushWord(u16 word);
    u16 popWord();
    u16 peekWord();

    enum class PageCrossed : bool {
	No = false,
	Yes = true,
    };
    struct AddressOperandType {
	AddressOperandType(u16 _address, bool _pageCrossed = false)
	    : address(_address)
	    , pageCrossed(_pageCrossed)
	{
	}
	operator u16() { return address; }
	u16 address;
	bool pageCrossed;
    };

    struct ValueOperandType {
	ValueOperandType(u8 _value, bool _pageCrossed = false)
	    : value(_value)
	    , pageCrossed(_pageCrossed)
	{
	}
	ValueOperandType(AddressOperandType address)
	    : value(Bus::the().readMemory(address.address))
	    , pageCrossed(address.pageCrossed)
	{
	}
	operator u8() { return value; }
	u8 value;
	bool pageCrossed;
    };
    void normallyIncrementClockCycle(MemoryAccessMode mode, bool pageCrossed = false);

    bool nmiPending() const { return mNMIPending; }
    void NMI();
    ProcessorStatus& P() { return mP; }
    u8& SP() { return mSP; }
    u16& PC() { return mPC; }
    u8& A() { return mA; }
    u8& X() { return mX; }
    u8& Y() { return mY; }
    ProcessorStatus P() const { return mP; }
    u8 SP() const { return mSP; }
    u16 PC() const { return mPC; }
    u8 A() const { return mA; }
    u8 X() const { return mX; }
    u8 Y() const { return mY; }
    void runOpcode(EncodedInstructionType);
private:
    u8 decode8Bits();
    u16 decode16Bits();

    ValueOperandType getOperand(MemoryAccessMode);
    AddressOperandType getAddressOperand(MemoryAccessMode);
    
    void setOrClearStatusIf(bool cond, ProcessorStatus);
    // Addition instructions do not currently set a status flag afterward.
    void BRK(MemoryAccessMode);
    void RTS(MemoryAccessMode);
    void RTI(MemoryAccessMode);
    void PHA(MemoryAccessMode);
    void PLP(MemoryAccessMode);
    void CLV(MemoryAccessMode);
    void CLC(MemoryAccessMode);
    void CLD(MemoryAccessMode);
    void CLI(MemoryAccessMode);

    enum class ShiftType {
	Rotate,
	Arithmetic,
    };
    void leftShiftImpl(MemoryAccessMode, ShiftType);
    void rightShiftImpl(MemoryAccessMode, ShiftType);
    void LSR(MemoryAccessMode);
    void ASL(MemoryAccessMode);
    void ROR(MemoryAccessMode);
    void ROL(MemoryAccessMode);

    void ORA(MemoryAccessMode);
    void AND(MemoryAccessMode);
    void ADC(MemoryAccessMode);
    void ADCImpl(ValueOperandType const& operand);
    void LDA(MemoryAccessMode);
    void EOR(MemoryAccessMode);
    void CMP(MemoryAccessMode);
    void CPX(MemoryAccessMode);
    void CPY(MemoryAccessMode);
    void SBC(MemoryAccessMode);
    void STA(MemoryAccessMode);
    void STX(MemoryAccessMode);
    void STY(MemoryAccessMode);
    void BIT(MemoryAccessMode);

    void DEC(MemoryAccessMode);
    void DEX(MemoryAccessMode);
    void DEY(MemoryAccessMode);
    void INC(MemoryAccessMode);
    void INX(MemoryAccessMode);
    void INY(MemoryAccessMode);
    void JMP(MemoryAccessMode);
    void LDX(MemoryAccessMode);
    void LDY(MemoryAccessMode);
    void JSR(MemoryAccessMode);
    void NOP(MemoryAccessMode);
    void SED(MemoryAccessMode);
    void SEC(MemoryAccessMode);
    void SEI(MemoryAccessMode);
    void PHP(MemoryAccessMode);
    void PLA(MemoryAccessMode);
    void BCS(MemoryAccessMode);
    void BCC(MemoryAccessMode);
    void BEQ(MemoryAccessMode);
    void BNE(MemoryAccessMode);
    void BVS(MemoryAccessMode);
    void BVC(MemoryAccessMode);
    void BPL(MemoryAccessMode);
    void BMI(MemoryAccessMode);

    void TAY(MemoryAccessMode);
    void TAX(MemoryAccessMode);
    void TXA(MemoryAccessMode);
    void TYA(MemoryAccessMode);
    void TSX(MemoryAccessMode);
    void TXS(MemoryAccessMode);

    
    /** These require memory access which we can't do right now
	void STA(MemoryAccessMode);
	
    */
    
    // DecodedInstruction decodeInstruction(EncodedInstructionType);
    void branchImpl(bool condition);
    

    void setProcessorStatus(ProcessorStatus);
    void clearProcessorStatus(ProcessorStatus);
    ALWAYS_INLINE bool processorStatusHas(ProcessorStatus status) { return (unsigned)mP & (unsigned)status; }
    ProcessorStatus mP;
    u8 mSP;
    u16 mPC;
    u8 mA;
    u8 mX;
    u8 mY;
    bool mNMIPending { false };

    bool mIsRunning;
    
    u32 mClockCycle;
};

}

using NESEmulator::CPU;
