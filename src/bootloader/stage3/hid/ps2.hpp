#pragma once

#include "mysty/compat.hpp"

namespace simpleos::hid {

bool initializePS2Driver();

} // namespace simpleos::hid

extern "C" {
__attribute__((interrupt)) void ps2Port1InterruptHandlerWrapper(void*);
ASM_CALLABLE void ps2Port1InterruptHandler();

__attribute__((interrupt)) void ps2Port2InterruptHandlerWrapper(void*);
ASM_CALLABLE void ps2Port2InterruptHandler();
}
