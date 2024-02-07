#pragma once

#include "mysty/int.hpp"

extern "C" {
void __attribute__((cdecl)) x86_outb(uint16_t port, uint8_t value);
uint8_t __attribute__((cdecl)) x86_inb(uint16_t port);

void __attribute__((cdecl)) x86_load_interrupt_table(void* table);
void __attribute__((cdecl)) x86_trivial_interrupt(void* interruptFrame);
}

inline void x86_io_wait() {
  // This port is pretty much unused
  x86_outb(0x80, 0);
}
