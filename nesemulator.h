#pragma once

#include "defs.h"
#include "forward.h" 
#include "cpu.h"
#include "bus.h"

class NESEmulator {
public:
    static NESEmulator& the();
    void powerOn();
    CPU& cpu() { return mCPU; }
    CPU const& cpu() const { return mCPU; }
    Bus& bus() { return mBus; }
    Bus const& bus() const { return mBus; }

private:
    NESEmulator();

    CPU mCPU { {} };
    Bus mBus { {} };
};
