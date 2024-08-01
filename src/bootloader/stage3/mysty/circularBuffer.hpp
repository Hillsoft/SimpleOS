#pragma once

#include "mysty/array.hpp"
#include "mysty/storage.hpp"
#include "mysty/typeattributes.hpp"

namespace mysty {

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
      for (size_t i = startIndex_; i != endIndex_; i = nextIndex(i)) {
        data_[i].destroy();
      }
    }
    startIndex_ = 0;
    endIndex_ = 0;
  }

  size_t size() const {
    if (endIndex_ >= startIndex_) {
      return endIndex_ - startIndex_;
    } else {
      return size_ - startIndex_ + endIndex_;
    }
  }

  template <typename... Args>
  void emplace_back(Args... args) {
    if (nextIndex(endIndex_) != startIndex_) {
      data_[endIndex_].emplace(mysty::forward<Args...>(args)...);
      endIndex_ = nextIndex(endIndex_);
    }
  }

  T& peek_front() { return *data_[startIndex_]; }
  const T& peek_front() const { return *data_[startIndex_]; }

  T pop_front() {
    StorageFor<T>& front = data_[startIndex_];
    T result = mysty::move(*front);
    if constexpr (non_trivially_destructible<T>) {
      front.destroy();
    }
    startIndex_ = nextIndex(startIndex_);
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
  size_t startIndex_ = 0;
  size_t endIndex_ = 0;
};

} // namespace mysty
