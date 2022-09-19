#pragma once

#include "assert.h"
#include "defs.h"
#include "forward.h"
#include "badge.h"


#define UNDEFINED_INSTRUCTION(instruction) \
    fprintf(stderr, "Instruction %08x not found\n", instruction);	\
    assert(false);


#define IS_ALU_OPCODE(opcode, offset, memoryAccessMode)		\
    case (unsigned)ALUInstructionOpcodeBase::opcode + offset:	\
    opcode(MemoryAccessMode::memoryAccessMode);	    \
    break



#define RUN_IF_ALU_OPCODE(opcode) \
    IS_ALU_OPCODE(opcode, 0x01, XZeroPageIndexed);	\
    IS_ALU_OPCODE(opcode, 0x05, ZeroPage);		\
    IS_ALU_OPCODE(opcode, 0x09, Immediate);		\
    IS_ALU_OPCODE(opcode, 0x0D, Absolute);		\
    IS_ALU_OPCODE(opcode, 0x11, YIndexedIndirect);	\
    IS_ALU_OPCODE(opcode, 0x15, XZeroPageIndexed);	\
    IS_ALU_OPCODE(opcode, 0x19, YAbsoluteIndexed);	\
    IS_ALU_OPCODE(opcode, 0x1D, XAbsoluteIndexed)


class CPU {
public:
    CPU(Badge<NESEmulator>) { } 	/* CPU can only be constructed/initialized inside an NESEmulator object */
    void reset(Badge<NESEmulator>);	/* Can the contents of this just be in the constructor? Is reset ever run outside of powering on the machine? Does it ever truly reboot? */

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
	XIndexedIndirect,       /* (d), x */
	YIndexedIndirect,       /* (d), y */
    };
    
    enum class ProcessorStatus { // TODO(Matt): research specific difference between Negative, Overflow, and Carry
	Carry,
	Zero,
	InterruptDisable,
	DecimalMode,
	BreakCommand,
	Overflow,
	Negative,
    };
    void normallyIncrementClockCycle(MemoryAccessMode mode);


    void execLoop();
    
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

    u8 getOperand(MemoryAccessMode);
    // Addition instructions do not currently set a status flag afterward.
    void ORA(MemoryAccessMode);
    void AND(MemoryAccessMode);
    void ADC(MemoryAccessMode);
    void EOR(MemoryAccessMode);
    void CMP(MemoryAccessMode);
    void SBC(MemoryAccessMode);

    void DEC(MemoryAccessMode);
    void DEX(MemoryAccessMode);
    void INC(MemoryAccessMode);
    void JMP(MemoryAccessMode);
    
    /** These require memory access which we can't do right now
	void STA(MemoryAccessMode);
	void LDA(MemoryAccessMode);
	
    */
    
    // DecodedInstruction decodeInstruction(EncodedInstructionType);

    
    ProcessorStatus mP;
    u8 mSP;
    u16 mPC;
    u8 mA;
    u8 mX;
    u8 mY;

    bool mIsRunning;
    
    u32 mClockCycle;
};
