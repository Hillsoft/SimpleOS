#pragma once

#include "mysty/int.hpp"

inline void* operator new(size_t n, void* ptr) {
  return ptr;
}
