#pragma once

#include "mystyint.hpp"

extern "C" {

void __attribute__((cdecl)) x86_outb(uint16_t port, uint8_t value);
void __attribute__((cdecl)) x86_inb(uint16_t port);

}