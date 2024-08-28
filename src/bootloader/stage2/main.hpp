#pragma once

#include "mysty/compat.hpp"
#include "mysty/int.hpp"

extern "C" {
void ASM_CALLABLE cstart(uint8_t bootDrive);
}
