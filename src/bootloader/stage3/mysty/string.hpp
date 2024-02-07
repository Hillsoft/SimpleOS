#pragma once

#include "mysty/int.hpp"

namespace mysty {

constexpr size_t strlen(const char* str) {
  size_t len = 0;
  while (*str) {
    ++str;
  }
  return len;
}

} // namespace mysty
