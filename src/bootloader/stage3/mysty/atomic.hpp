#pragma once

#include "mysty/typeattributes.hpp"

namespace mysty {

enum class MemoryOrder {
  Relaxed,
  Acquire,
  Release,
  AcquireRelease,
  SequentialConsistency,
};

template <typename T>
  requires(integral<T> || pointer<T>)
class Atomic {
 public:
  constexpr Atomic(T value) : value_(value) {}

  Atomic(const Atomic& other) = delete;
  Atomic(Atomic&& other) = delete;

  Atomic& operator=(const Atomic& other) = delete;
  Atomic& operator=(Atomic&& ohter) = delete;

  T load(MemoryOrder order = MemoryOrder::SequentialConsistency) const {
    T result;
    __atomic_load(&value_, &result, mapMemoryOrder(order));
    return result;
  }

  void store(
      T newValue, MemoryOrder order = MemoryOrder::SequentialConsistency) {
    __atomic_store(&value_, &newValue, mapMemoryOrder(order));
  }

 private:
  static int mapMemoryOrder(MemoryOrder order) {
    switch (order) {
      case MemoryOrder::Relaxed:
        return __ATOMIC_RELAXED;
      case MemoryOrder::Acquire:
        return __ATOMIC_ACQUIRE;
      case MemoryOrder::Release:
        return __ATOMIC_RELEASE;
      case MemoryOrder::AcquireRelease:
        return __ATOMIC_ACQ_REL;
      case MemoryOrder::SequentialConsistency:
        return __ATOMIC_SEQ_CST;
      default:
        return __ATOMIC_SEQ_CST;
    }
  }

  T value_;
};

} // namespace mysty
