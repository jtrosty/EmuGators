#pragma once

#include "defs.h"
#include "bus.h"
#include "forward.h" 
#include "cpu.h"

namespace NESEmulator {

    u32 clockCycle = 0;
    u16 addrNMI = 0xFFFA;

    void powerOn();
    void execLoop();

};


