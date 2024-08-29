#pragma once

#include "mysty/array.hpp"
#include "mysty/atomic.hpp"
#include "mysty/storage.hpp"
#include "mysty/typeattributes.hpp"

namespace mysty {

/*
 * Supports single reader + single writer concurrency.
 * clear() cannot be called concurrently with any other access
 */
template <typename T, size_t size_>
  requires is_not_const<T>
class FixedCircularBuffer {
 public:
  explicit constexpr FixedCircularBuffer() {}

  ~FixedCircularBuffer()
    requires(non_trivially_destructible<T>)
  {
    clear();
  }

  ~FixedCircularBuffer()
    requires(trivially_destructible<T>)
  = default;

  void clear() {
    if constexpr (non_trivially_destructible<T>) {
      size_t curEnd = endIndex_.load(MemoryOrder::Acquire);
      for (size_t i = startIndex_.load(MemoryOrder::Acquire); i != curEnd;
           i = nextIndex(i)) {
        data_[i].destroy();
      }
    }
    startIndex_.store(0, mysty::MemoryOrder::Release);
    endIndex_.store(0, mysty::MemoryOrder::Release);
  }

  bool empty() const {
    return startIndex_.load(MemoryOrder::Relaxed) ==
        endIndex_.load(MemoryOrder::Relaxed);
  }

  size_t size() const {
    size_t curStart = startIndex_.load(MemoryOrder::Relaxed);
    size_t curEnd = endIndex_.load(MemoryOrder::Relaxed);
    if (curEnd >= curStart) {
      return curEnd - curStart;
    } else {
      return size_ - curStart + curEnd;
    }
  }

  template <typename... Args>
  void emplace_back(Args... args) {
    size_t curStart = startIndex_.load(MemoryOrder::Acquire);
    size_t curEnd = endIndex_.load(MemoryOrder::Acquire);
    if (nextIndex(curEnd) != curStart) {
      data_[curEnd].emplace(mysty::forward<Args...>(args)...);
      endIndex_.store(nextIndex(curEnd), MemoryOrder::Release);
    }
  }

  T& peek_front() {
    endIndex_.load(MemoryOrder::Acquire);
    return *data_[startIndex_.load(MemoryOrder::Acquire)];
  }
  const T& peek_front() const {
    endIndex_.load(MemoryOrder::Acquire);
    return *data_[startIndex_.load(MemoryOrder::Acquire)];
  }

  T pop_front() {
    endIndex_.load(MemoryOrder::Acquire);
    size_t curStart = startIndex_.load(MemoryOrder::Acquire);
    StorageFor<T>& front = data_[curStart];
    T result = mysty::move(*front);
    if constexpr (non_trivially_destructible<T>) {
      front.destroy();
    }
    startIndex_.store(nextIndex(curStart), MemoryOrder::Release);
    return result;
  }

 private:
  size_t nextIndex(size_t i) const {
    i++;
    if (i == size_) {
      return 0;
    }
    return i;
  }

  FixedArray<StorageFor<T>, size_> data_;
  mysty::Atomic<size_t> startIndex_ = 0;
  mysty::Atomic<size_t> endIndex_ = 0;
};

} // namespace mysty
