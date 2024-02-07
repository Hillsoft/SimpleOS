#pragma once

#include "mysty/int.hpp"

namespace simpleos {

enum class InterruptType {
  Interrupt,
  Trap,
};

void initializeInterrupts();
void registerInterrupt(
    uint8_t offset,
    __attribute__((interrupt)) void (*handler)(void*),
    InterruptType type);

} // namespace simpleos
