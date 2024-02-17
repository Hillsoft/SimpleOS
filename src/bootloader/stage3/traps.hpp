#pragma once

namespace simpleos {

void registerTraps();

} // namespace simpleos

extern "C" {
__attribute__((interrupt)) void divErrorInterruptHandlerWrapper(void*);
__attribute__((cdecl)) __attribute__((externally_visible)) void
divErrorInterruptHandler(void* faultingAddress);
}
