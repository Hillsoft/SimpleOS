#pragma once

#include "mysty/int.hpp"

namespace simpleos {

enum class InterruptType {
  Interrupt,
  Trap,
};

enum class InterruptRange { PIC, CPU };

void initializeInterrupts();
void registerInterrupt(
    uint8_t offset,
    __attribute__((interrupt)) void (*handler)(void*),
    InterruptType type,
    InterruptRange range);

inline void awaitInterrupt() {
  asm volatile("hlt" : : : "memory");
}

} // namespace simpleos
