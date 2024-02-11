#pragma once

#include "mysty/int.hpp"

namespace mysty {

inline void memcpy(uint8_t* destination, const uint8_t* source, size_t count) {
  for (size_t i = 0; i < count; i++) {
    destination[i] = source[i];
  }
}

} // namespace mysty
