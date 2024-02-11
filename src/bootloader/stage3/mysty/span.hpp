#pragma once

#include "mysty/concepts/iterable.hpp"
#include "mysty/concepts/orderable.hpp"
#include "mysty/int.hpp"
#include "mysty/optional.hpp"
#include "mysty/string.hpp"
#include "mysty/typeattributes.hpp"

namespace mysty {

template <typename T>
class Span {
 private:
  class Iterator {
   public:
    using value_t = T;

    constexpr Iterator(T* current) : current_(current) {}

    constexpr bool operator==(Iterator const& other) const {
      return current_ == other.current_;
    }

    constexpr T& operator*() const { return *current_; }

    constexpr Iterator const& operator++() {
      current_ += 1;
      return *this;
    }

    constexpr operator T*() const { return current_; }

   protected:
    T* current_;
  };

 public:
  using iterator_t = Iterator;

  constexpr Span() : start_(nullptr), end_(nullptr) {}
  constexpr Span(T* start, T* end) : start_(start), end_(end) {}
  constexpr Span(T* start, size_t count) : start_(start), end_(start + count) {}

  template <typename T2 = T>
    requires(same_as<T, char>)
  constexpr Span(char* c) : Span(c, strlen(c)) {}

  template <typename T2 = T>
    requires(same_as<T, const char>)
  constexpr Span(const char* c) : Span(c, strlen(c)) {}

  constexpr T& at(size_t i) const { return start_[i]; }
  constexpr T& operator[](size_t i) const { return start_[i]; }

  constexpr iterator_t begin() const { return iterator_t{start_}; }
  constexpr iterator_t end() const { return iterator_t{end_}; }

  constexpr size_t size() const { return static_cast<size_t>(end_ - start_); }

  constexpr Span slice_front(
      size_t offset, size_t count = int_limits<size_t>::max) const {
    size_t actualCount = min(count, size() - offset);
    return Span{start_ + offset, actualCount};
  }

  template <typename T2 = T>
    requires(HasEquality<T>)
  constexpr bool operator==(const Span& other) const {
    if (other.size() != size()) {
      return false;
    }
    for (size_t i = 0; i < size(); ++i) {
      if (at(i) != other.at(i)) {
        return false;
      }
    }

    return true;
  }

  template <typename T2 = T>
    requires(HasEquality<T>)
  constexpr Optional<size_t> find(const T& needle) const {
    for (size_t i = 0; i < size(); ++i) {
      if (at(i) == needle) {
        return i;
      }
    }
    return {};
  }

  template <typename T2>
  constexpr Span<T2> reinterpret() const {
    return Span<T2>{reinterpret_cast<T2*>(start_), reinterpret_cast<T2*>(end_)};
  }

 protected:
  T* start_;
  T* end_;
};

using StringView = Span<const char>;

} // namespace mysty
