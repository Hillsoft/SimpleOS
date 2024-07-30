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

  registerInterrupt(
      10,
      invalidTssInterruptHandlerWrapper,
      InterruptType::Trap,
      InterruptRange::CPU);

  registerInterrupt(
      11,
      segmentNotPresentInterruptHandlerWrapper,
      InterruptType::Trap,
      InterruptRange::CPU);

  registerInterrupt(
      12,
      stackSegmentFaultInterruptHandlerWrapper,
      InterruptType::Trap,
      InterruptRange::CPU);

  registerInterrupt(
      13,
      generalProtectionFaultInterruptHandlerWrapper,
      InterruptType::Trap,
      InterruptRange::CPU);
}

SelectorErrorCode::SelectorErrorCode(uint32_t rawErrorCode)
    : rawErrorCode_(rawErrorCode) {}

bool SelectorErrorCode::external() const {
  return (rawErrorCode_ & 0b1) > 0;
}

SelectorErrorCode::Table SelectorErrorCode::table() const {
  auto tableIndex = (rawErrorCode_ & 0b110) >> 1;
  switch (tableIndex) {
    case 0:
      return Table::GDT;
    case 1:
      return Table::IDT;
    case 2:
      return Table::LDT;
    default:
      return Table::IDT;
  }
}

mysty::StringView SelectorErrorCode::tableStr() const {
  switch (table()) {
    case Table::GDT:
      return "GDT";
    case Table::IDT:
      return "IDT";
    case Table::LDT:
      return "LDT";
    default:
      return "Unknown table";
  }
}

uint16_t SelectorErrorCode::index() const {
  return (rawErrorCode_ >> 3) & 0x1FFF;
}

} // namespace simpleos

extern "C" {
ASM_CALLABLE void divErrorInterruptHandler(void* faultingAddress) {
  mysty::printf(
      "\nFault at: 0x%X\nDivision by zero\n",
      reinterpret_cast<size_t>(faultingAddress));
  abort();
}

ASM_CALLABLE void debugInterruptHandler(void* faultingAddress) {
  mysty::printf(
      "\nFault at: 0x%X\nDebug signal\n",
      reinterpret_cast<size_t>(faultingAddress));
  abort();
}

ASM_CALLABLE void nmiInterruptHandler(void* faultingAddress) {
  mysty::printf(
      "\nFault at: 0x%X\nNMI\n", reinterpret_cast<size_t>(faultingAddress));
  abort();
}

ASM_CALLABLE void breakpointInterruptHandler(void* faultingAddress) {
  mysty::printf(
      "\nFault at: 0x%X\nBreakpoint\n",
      reinterpret_cast<size_t>(faultingAddress));
  abort();
}

ASM_CALLABLE void overflowInterruptHandler(void* faultingAddress) {
  mysty::printf(
      "\nFault at: 0x%X\nOverflow\n",
      reinterpret_cast<size_t>(faultingAddress));
  abort();
}

ASM_CALLABLE void boundRangeExceededInterruptHandler(void* faultingAddress) {
  mysty::printf(
      "\nFault at: 0x%X\nBound range exceeded\n",
      reinterpret_cast<size_t>(faultingAddress));
  abort();
}

ASM_CALLABLE void invalidOpcodeInterruptHandler(void* faultingAddress) {
  mysty::printf(
      "\nFault at: 0x%X\nInvalid opcode\n",
      reinterpret_cast<size_t>(faultingAddress));
  abort();
}

ASM_CALLABLE void fpuNotAvailableInterruptHandler(void* faultingAddress) {
  mysty::printf(
      "\nFault at: 0x%X\nFPU not available\n",
      reinterpret_cast<size_t>(faultingAddress));
  abort();
}

ASM_CALLABLE void doubleFaultInterruptHandler(
    void* faultingAddress, uint32_t errorCode) {
  mysty::printf(
      "\nFault at: 0x%X\nDouble fault\nError code: 0x%X\n",
      reinterpret_cast<size_t>(faultingAddress),
      errorCode);
  abort();
}

ASM_CALLABLE void invalidTssInterruptHandler(
    void* faultingAddress, simpleos::SelectorErrorCode errorCode) {
  mysty::printf(
      "\nFault at: 0x%X\nInvalid TSS\nIndex 0x%X in %s",
      reinterpret_cast<size_t>(faultingAddress),
      errorCode.index(),
      errorCode.tableStr());
  abort();
}

ASM_CALLABLE void segmentNotPresentInterruptHandler(
    void* faultingAddress, simpleos::SelectorErrorCode errorCode) {
  mysty::printf(
      "\nFault at: 0x%X\nSegment Not Found\nIndex 0x%X in %s",
      reinterpret_cast<size_t>(faultingAddress),
      errorCode.index(),
      errorCode.tableStr());
  abort();
}

ASM_CALLABLE void stackSegmentFaultInterruptHandler(
    void* faultingAddress, simpleos::SelectorErrorCode errorCode) {
  mysty::printf(
      "\nFault at: 0x%X\nStack Segment Fault\nIndex 0x%X in %s",
      reinterpret_cast<size_t>(faultingAddress),
      errorCode.index(),
      errorCode.tableStr());
  abort();
}

ASM_CALLABLE void generalProtectionFaultInterruptHandler(
    void* faultingAddress, simpleos::SelectorErrorCode errorCode) {
  mysty::printf(
      "\nFault at: 0x%X\nGeneral Protection Fault\nIndex 0x%X in %s",
      reinterpret_cast<size_t>(faultingAddress),
      errorCode.index(),
      errorCode.tableStr());
  abort();
}
}
