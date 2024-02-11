#pragma once

#include "mysty/int.hpp"

namespace mysty {

constexpr size_t strlen(const char* str) noexcept {
  size_t len = 0;
  while (str[len] != 0) {
    ++len;
  }
  return len;
}

} // namespace mysty
