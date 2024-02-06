#pragma once

#include "mysty/int.hpp"

namespace mysty {

template <typename T, uint32_t size_>
class FixedArray {
 private:
  template <typename IT, typename IArr>
  class Iterator {
   public:
    using value_t = IT;

    Iterator(IArr& array, uint32_t index) : array_(array), index_(index) {}

    bool operator==(Iterator const& other) const {
      return &array_ == &other.array_ && index_ == other.index_;
    }

    IT& operator*() const { return array_[index_]; }

    Iterator const& operator++() {
      index_ += 1;
      return *this;
    }

   protected:
    IArr& array_;
    uint32_t index_;
  };

 public:
  using iterator_t = Iterator<T, FixedArray>;
  using const_iterator_t = Iterator<const T, const FixedArray>;

  FixedArray() {}

  FixedArray(const T& defaultVal) {
    for (uint32_t i = 0; i < size_; i++) {
      data[i] = defaultVal;
    }
  }

  template <typename... Args>
  FixedArray(Args... args) {
    initializeFromPack<0>(args...);
  }

  uint32_t size() const { return size_; }

  T& at(uint32_t i) { return data[i]; }
  const T& at(uint32_t i) const { return data[i]; }

  T& operator[](uint32_t i) { return data[i]; }
  const T& operator[](uint32_t i) const { return data[i]; }

  iterator_t begin() { return iterator_t{*this, 0}; }
  const_iterator_t begin() const { return const_iterator_t{*this, 0}; }

  iterator_t end() { return iterator_t{*this, size_}; }
  const_iterator_t end() const { return const_iterator_t{*this, size_}; }

 private:
  template <uint32_t i, typename Arg, typename... Args>
  void initializeFromPack(Arg a, Args... args) {
    static_assert(
        i < size_, "Too many arguments used to initialize FixedArray");
    data[i] = a;
    initializeFromPack<i + 1>(args...);
  }

  template <uint32_t i>
  void initializeFromPack() {
    static_assert(
        i == size_, "Too few arguments used to initialize FixedArray");
  }

  T data[size_];
};

template <typename T>
class FixedArray<T, 0> {
 private:
  template <typename IT>
  class NullIterator {
   public:
    using value_t = IT;

    bool operator==(NullIterator const& other) { return true; }
    bool operator!=(NullIterator const& other) { return false; }

    IT& operator*() const { __builtin_unreachable(); }

    NullIterator const& operator++() { return *this; }
  };

 public:
  using iterator_t = NullIterator<T>;
  using const_iterator_t = NullIterator<const T>;

  uint32_t size() const { return 0; }

  iterator_t begin() { return iterator_t{}; }
  iterator_t end() { return iterator_t{}; }

  const_iterator_t begin() const { return const_iterator_t{}; }
  const_iterator_t end() const { return const_iterator_t{}; }
};

} // namespace mysty
