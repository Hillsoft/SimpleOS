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
}
