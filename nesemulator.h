#pragma once

#include "defs.h"
#include "bus.h"
#include "forward.h" 
#include "cpu.h"

class NESEmulator {
public:
    static NESEmulator& the();
    void powerOn();
    CPU& cpu() { return mCPU; }
    CPU const& cpu() const { return mBus; }
    Bus& bus() { return mCPU; }
    Bus const& bus() const { return mBus; }
private:
    NESEmulator();

    CPU mCPU { {} };
    Bus mBus { {} };
};
