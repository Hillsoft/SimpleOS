#pragma once

#include "mysty/int.hpp"

namespace simpleos {

void registerTraps();

} // namespace simpleos

extern "C" {
__attribute__((interrupt)) void divErrorInterruptHandlerWrapper(void*);
__attribute__((cdecl)) __attribute__((externally_visible)) void
divErrorInterruptHandler(void* faultingAddress);

__attribute__((interrupt)) void debugInterruptHandlerWrapper(void*);
__attribute__((cdecl)) __attribute__((externally_visible)) void
debugInterruptHandler(void* faultingAddress);

__attribute__((interrupt)) void nmiInterruptHandlerWrapper(void*);
__attribute__((cdecl)) __attribute__((externally_visible)) void
nmiInterruptHandler(void* faultingAddress);

__attribute__((interrupt)) void breakpointInterruptHandlerWrapper(void*);
__attribute__((cdecl)) __attribute__((externally_visible)) void
breakpointInterruptHandler(void* faultingAddress);

__attribute__((interrupt)) void overflowInterruptHandlerWrapper(void*);
__attribute__((cdecl)) __attribute__((externally_visible)) void
overflowInterruptHandler(void* faultingAddress);

__attribute__((interrupt)) void boundRangeExceededInterruptHandlerWrapper(
    void*);
__attribute__((cdecl)) __attribute__((externally_visible)) void
boundRangeExceededInterruptHandler(void* faultingAddress);

__attribute__((interrupt)) void invalidOpcodeInterruptHandlerWrapper(void*);
__attribute__((cdecl)) __attribute__((externally_visible)) void
invalidOpcodeInterruptHandler(void* faultingAddress);

__attribute__((interrupt)) void fpuNotAvailableInterruptHandlerWrapper(void*);
__attribute__((cdecl)) __attribute__((externally_visible)) void
fpuNotAvailableInterruptHandler(void* faultingAddress);

__attribute__((interrupt)) void doubleFaultInterruptHandlerWrapper(void*);
__attribute__((cdecl)) __attribute__((externally_visible)) void
doubleFaultInterruptHandler(void* faultingAddress, uint32_t errorCode);
}
