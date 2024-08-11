#pragma once

#include "mysty/int.hpp"

extern "C" {
void __attribute__((sysv_abi)) x86_load_interrupt_table(void* table);
void __attribute__((sysv_abi)) x86_trivial_interrupt(void* interruptFrame);
}

inline void x86_outb(uint16_t port, uint8_t value) {
  asm volatile("out %b[value], %w[port]"
               : /* no output */
               : [port] "d"(port), [value] "a"(value));
}

inline uint8_t x86_inb(uint16_t port) {
  uint8_t result = 0;
  asm volatile("in %w[port], %b[result] "
               : [result] "+a"(result)
               : [port] "d"(port));
  return result;
}

inline void x86_io_wait() {
  // This port is pretty much unused
  x86_outb(0x80, 0);
}
