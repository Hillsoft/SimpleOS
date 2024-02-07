#pragma once

#include "mysty/int.hpp"
#include "mysty/span.hpp"
#include "mysty/typeattributes.hpp"

namespace mysty {

template <typename T, size_t size_>
  requires is_not_const<T>
class FixedArray {
 private:
  template <typename IT, typename IArr>
  class Iterator {
   public:
    using value_t = IT;

    constexpr Iterator(IArr& array, size_t index)
        : array_(array), index_(index) {}

    constexpr bool operator==(Iterator const& other) const {
      return &array_ == &other.array_ && index_ == other.index_;
    }

    constexpr IT& operator*() const { return array_[index_]; }

    constexpr Iterator const& operator++() {
      index_ += 1;
      return *this;
    }

   protected:
    IArr& array_;
    size_t index_;
  };

 public:
  using iterator_t = Iterator<T, FixedArray>;
  using const_iterator_t = Iterator<const T, const FixedArray>;

  constexpr FixedArray() {}

  explicit constexpr FixedArray(const T& defaultVal) {
    for (size_t i = 0; i < size_; i++) {
      data[i] = defaultVal;
    }
  }

  template <typename... Args>
  explicit constexpr FixedArray(Args... args) {
    initializeFromPack<0>(args...);
  }

  constexpr size_t size() const { return size_; }

  constexpr T& at(size_t i) { return data[i]; }
  constexpr const T& at(size_t i) const { return data[i]; }

  constexpr T& operator[](size_t i) { return data[i]; }
  constexpr const T& operator[](size_t i) const { return data[i]; }

  constexpr iterator_t begin() { return iterator_t{*this, 0}; }
  constexpr const_iterator_t begin() const {
    return const_iterator_t{*this, 0};
  }

  constexpr iterator_t end() { return iterator_t{*this, size_}; }
  constexpr const_iterator_t end() const {
    return const_iterator_t{*this, size_};
  }

  constexpr operator Span<T>() { return Span<T>{data, data + size_}; }
  constexpr operator Span<T const>() const {
    return Span<T const>{data, data + size_};
  }

 private:
  template <size_t i, typename Arg, typename... Args>
  constexpr void initializeFromPack(Arg a, Args... args) {
    static_assert(
        i < size_, "Too many arguments used to initialize FixedArray");
    data[i] = a;
    initializeFromPack<i + 1>(args...);
  }

  template <size_t i>
  constexpr void initializeFromPack() {
    static_assert(
        i == size_, "Too few arguments used to initialize FixedArray");
  }

  T data[size_];
};

template <typename T>
  requires is_not_const<T>
class FixedArray<T, 0> {
 private:
  template <typename IT>
  class NullIterator {
   public:
    using value_t = IT;

    constexpr bool operator==(NullIterator const& other) { return true; }
    constexpr bool operator!=(NullIterator const& other) { return false; }

    constexpr IT& operator*() const { __builtin_unreachable(); }

    constexpr NullIterator const& operator++() { return *this; }
  };

 public:
  using iterator_t = NullIterator<T>;
  using const_iterator_t = NullIterator<const T>;

  constexpr size_t size() const { return 0; }

  constexpr iterator_t begin() { return iterator_t{}; }
  constexpr iterator_t end() { return iterator_t{}; }

  constexpr const_iterator_t begin() const { return const_iterator_t{}; }
  constexpr const_iterator_t end() const { return const_iterator_t{}; }

  constexpr operator Span<T>() { return Span<T>{}; }
  constexpr operator Span<T const>() const { return Span<T const>{}; }
};

} // namespace mysty
