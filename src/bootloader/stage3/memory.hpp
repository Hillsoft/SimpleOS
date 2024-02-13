#pragma once

#include "mysty/int.hpp"

namespace simpleos {

bool initializeMemory();

void* malloc(size_t size);
void free(void* ptr);

} // namespace simpleos
