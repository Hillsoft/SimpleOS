#pragma once

#include "mysty/int.hpp"

extern "C" {
void __attribute__((cdecl)) __attribute__((externally_visible))
cstart(uint8_t bootDrive);
}
