#include "traps.hpp"

#include "interrupts.hpp"
#include "mysty/io.hpp"

namespace {

void abort() {
  constexpr mysty::StringView abortMessage = "Aborting!";
  mysty::puts(abortMessage);
  while (true) {
    asm volatile("hlt");
  }
}

} // namespace

namespace simpleos {

void registerTraps() {
  registerInterrupt(
      0,
      divErrorInterruptHandlerWrapper,
      InterruptType::Trap,
      InterruptRange::CPU);

  registerInterrupt(
      1,
      debugInterruptHandlerWrapper,
      InterruptType::Trap,
      InterruptRange::CPU);

  registerInterrupt(
      2, nmiInterruptHandlerWrapper, InterruptType::Trap, InterruptRange::CPU);

  registerInterrupt(
      3,
      breakpointInterruptHandlerWrapper,
      InterruptType::Trap,
      InterruptRange::CPU);

  registerInterrupt(
      4,
      overflowInterruptHandlerWrapper,
      InterruptType::Trap,
      InterruptRange::CPU);

  registerInterrupt(
      5,
      boundRangeExceededInterruptHandlerWrapper,
      InterruptType::Trap,
      InterruptRange::CPU);

  registerInterrupt(
      6,
      invalidOpcodeInterruptHandlerWrapper,
      InterruptType::Trap,
      InterruptRange::CPU);

  registerInterrupt(
      7,
      fpuNotAvailableInterruptHandlerWrapper,
      InterruptType::Trap,
      InterruptRange::CPU);

  registerInterrupt(
      8,
      doubleFaultInterruptHandlerWrapper,
      InterruptType::Trap,
      InterruptRange::CPU);
}

} // namespace simpleos

extern "C" {
__attribute__((cdecl)) __attribute__((externally_visible)) void
divErrorInterruptHandler(void* faultingAddress) {
  mysty::printf(
      "\nFault at: 0x%X\nDivision by zero\n",
      reinterpret_cast<size_t>(faultingAddress));
  abort();
}

__attribute__((cdecl)) __attribute__((externally_visible)) void
debugInterruptHandler(void* faultingAddress) {
  mysty::printf(
      "\nFault at: 0x%X\nDebug signal\n",
      reinterpret_cast<size_t>(faultingAddress));
  abort();
}

__attribute__((cdecl)) __attribute__((externally_visible)) void
nmiInterruptHandler(void* faultingAddress) {
  mysty::printf(
      "\nFault at: 0x%X\nNMI\n", reinterpret_cast<size_t>(faultingAddress));
  abort();
}

__attribute__((cdecl)) __attribute__((externally_visible)) void
breakpointInterruptHandler(void* faultingAddress) {
  mysty::printf(
      "\nFault at: 0x%X\nBreakpoint\n",
      reinterpret_cast<size_t>(faultingAddress));
  abort();
}

__attribute__((cdecl)) __attribute__((externally_visible)) void
overflowInterruptHandler(void* faultingAddress) {
  mysty::printf(
      "\nFault at: 0x%X\nOverflow\n",
      reinterpret_cast<size_t>(faultingAddress));
  abort();
}

__attribute__((cdecl)) __attribute__((externally_visible)) void
boundRangeExceededInterruptHandler(void* faultingAddress) {
  mysty::printf(
      "\nFault at: 0x%X\nBound range exceeded\n",
      reinterpret_cast<size_t>(faultingAddress));
  abort();
}

__attribute__((cdecl)) __attribute__((externally_visible)) void
invalidOpcodeInterruptHandler(void* faultingAddress) {
  mysty::printf(
      "\nFault at: 0x%X\nInvalid opcode\n",
      reinterpret_cast<size_t>(faultingAddress));
  abort();
}

__attribute__((cdecl)) __attribute__((externally_visible)) void
fpuNotAvailableInterruptHandler(void* faultingAddress) {
  mysty::printf(
      "\nFault at: 0x%X\nFPU not available\n",
      reinterpret_cast<size_t>(faultingAddress));
  abort();
}

__attribute__((cdecl)) __attribute__((externally_visible)) void
doubleFaultInterruptHandler(void* faultingAddress, uint32_t errorCode) {
  mysty::printf(
      "\nFault at: 0x%X\nDouble fault\nError code: 0x%X\n",
      reinterpret_cast<size_t>(faultingAddress),
      errorCode);
  abort();
}
}
