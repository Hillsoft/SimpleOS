#pragma once

#include "mysty/compat.hpp"
#include "mysty/int.hpp"
#include "mysty/span.hpp"

namespace simpleos {

void registerTraps();

struct SelectorErrorCode {
 public:
  enum class Table { GDT, IDT, LDT };

  SelectorErrorCode(uint32_t rawErrorCode);

  bool external() const;
  Table table() const;
  mysty::StringView tableStr() const;
  uint16_t index() const;

 private:
  uint32_t rawErrorCode_;
};
static_assert(sizeof(SelectorErrorCode) == 4);

struct PageFaultErrorCode {
 public:
  PageFaultErrorCode(uint32_t rawErrorCode);

  bool present() const;
  bool write() const;
  bool read() const;
  bool user() const;
  bool reservedWrite() const;
  bool execute() const;
  bool protectionKey() const;
  bool shadowStack() const;
  bool sgx() const;

 private:
  uint32_t rawErrorCode_;
};
static_assert(sizeof(PageFaultErrorCode) == 4);

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

__attribute__((interrupt)) void invalidTssInterruptHandlerWrapper(void*);
ASM_CALLABLE void invalidTssInterruptHandler(
    void* faultingAddress, simpleos::SelectorErrorCode errorCode);

__attribute__((interrupt)) void segmentNotPresentInterruptHandlerWrapper(void*);
ASM_CALLABLE void segmentNotPresentInterruptHandler(
    void* faultingAddress, simpleos::SelectorErrorCode errorCode);

__attribute__((interrupt)) void stackSegmentFaultInterruptHandlerWrapper(void*);
ASM_CALLABLE void stackSegmentFaultInterruptHandler(
    void* faultingAddress, simpleos::SelectorErrorCode errorCode);

__attribute__((interrupt)) void generalProtectionFaultInterruptHandlerWrapper(
    void*);
ASM_CALLABLE void generalProtectionFaultInterruptHandler(
    void* faultingAddress, simpleos::SelectorErrorCode errorCode);

__attribute__((interrupt)) void pageFaultInterruptHandlerWrapper(void*);
ASM_CALLABLE void pageFaultInterruptHandler(
    void* faultingAddress, simpleos::PageFaultErrorCode errorCode);
}
