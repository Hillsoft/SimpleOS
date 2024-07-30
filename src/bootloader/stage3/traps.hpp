#pragma once

#include "mysty/compat.hpp"
#include "mysty/int.hpp"

namespace simpleos {

void registerTraps();

struct SelectorErrorCode {
 public:
  enum class Table { GDT, IDT, LDT };

  SelectorErrorCode(uint32_t rawErrorCode);

  bool external() const;
  Table table() const;
  uint16_t index() const;

 private:
  uint32_t rawErrorCode_;
};
static_assert(sizeof(SelectorErrorCode) == 4);

} // namespace simpleos

extern "C" {
__attribute__((interrupt)) void divErrorInterruptHandlerWrapper(void*);
ASM_CALLABLE void divErrorInterruptHandler(void* faultingAddress);

__attribute__((interrupt)) void debugInterruptHandlerWrapper(void*);
ASM_CALLABLE void debugInterruptHandler(void* faultingAddress);

__attribute__((interrupt)) void nmiInterruptHandlerWrapper(void*);
ASM_CALLABLE void nmiInterruptHandler(void* faultingAddress);

__attribute__((interrupt)) void breakpointInterruptHandlerWrapper(void*);
ASM_CALLABLE void breakpointInterruptHandler(void* faultingAddress);

__attribute__((interrupt)) void overflowInterruptHandlerWrapper(void*);
ASM_CALLABLE void overflowInterruptHandler(void* faultingAddress);

__attribute__((interrupt)) void boundRangeExceededInterruptHandlerWrapper(
    void*);
ASM_CALLABLE void boundRangeExceededInterruptHandler(void* faultingAddress);

__attribute__((interrupt)) void invalidOpcodeInterruptHandlerWrapper(void*);
ASM_CALLABLE void invalidOpcodeInterruptHandler(void* faultingAddress);

__attribute__((interrupt)) void fpuNotAvailableInterruptHandlerWrapper(void*);
ASM_CALLABLE void fpuNotAvailableInterruptHandler(void* faultingAddress);

__attribute__((interrupt)) void doubleFaultInterruptHandlerWrapper(void*);
ASM_CALLABLE void doubleFaultInterruptHandler(
    void* faultingAddress, uint32_t errorCode);
}
