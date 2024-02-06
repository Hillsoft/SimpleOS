#pragma once

#include "mysty/int.hpp"

extern "C" {
void __attribute__((cdecl)) cstart(uint8_t bootDrive);
}
