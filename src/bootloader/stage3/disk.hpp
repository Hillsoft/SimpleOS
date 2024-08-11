#pragma once

#include "mysty/compat.hpp"
#include "mysty/int.hpp"
#include "mysty/span.hpp"

namespace simpleos::disk {

bool initialize(uint8_t driveNumber);

size_t read(uint32_t lba, mysty::Span<uint8_t> outBuffer);

extern "C" {
__attribute__((interrupt)) void floppyInterruptHandlerWrapper(void*);
ASM_CALLABLE void floppyInterruptHandler();
}

} // namespace simpleos::disk
