#pragma once

#include "assert.h"
#include "defs.h"
#include "forward.h"
#include "badge.h"


#define UNDEFINED_MEMORY_ACCESS_MODE(mode) \
    fprintf(stderr, "Memory access code number %u failed\n", mode);	\
    assert(false);


#define IS_ALU_OPCODE(opcode, offset, memoryAccessMode)		\
    case (unsigned)ALUInstructionOpcodeBase::opcode + offset:	\
    opcode(MemoryAccessMode::memoryAccessMode);	    \
    break;



#define RUN_IF_ALU_OPCODE(opcode) \
    IS_ALU_OPCODE(opcode, 0x01, XZeroPageIndexed) \
    IS_ALU_OPCODE(opcode, 0x05, ZeroPage) \
    IS_ALU_OPCODE(opcode, 0x09, Immediate) \
    IS_ALU_OPCODE(opcode, 0x0D, Absolute) \
    IS_ALU_OPCODE(opcode, 0x11, YIndexedIndirect) \
    IS_ALU_OPCODE(opcode, 0x15, XZeroPageIndexed) \
    IS_ALU_OPCODE(opcode, 0x19, YAbsoluteIndexed) \
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

    
    enum MemoryAccessMode {
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
    
    
    u8& P() { return mP; }
    u8& SP() { return mSP; }
    u16& PC() { return mPC; }
    u8& A() { return mA; }
    u8& X() { return mX; }
    u8& Y() { return mY; }
    u8 P() const { return mP; }
    u8 SP() const { return mSP; }
    u16 PC() const { return mPC; }
    u8 A() const { return mA; }
    u8 X() const { return mX; }
    u8 Y() const { return mY; }
    void runOpcode(EncodedInstructionType);

private:
    void AND(MemoryAccessMode);
    // DecodedInstruction decodeInstruction(EncodedInstructionType);

    
    u8 mP;
    u8 mSP;
    u16 mPC;;
    u8 mA;
    u8 mX;
    u8 mY;
    
    u32 mClockCycle;
};
