#pragma once

#include "defs.h"
#include "romloader.h"
#include "cpu.h"
#include "ppu.h"


namespace NESEmulator {

    // auto& bus = Bus::the();
    // auto& cpu = CPU::the();
    // auto& ppu = PPU::the();

    void powerOn(RomLoader &romloader);

};


