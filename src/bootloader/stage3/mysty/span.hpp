#pragma once

#include "mysty/concepts/iterable.hpp"
#include "mysty/int.hpp"
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

 protected:
  T* start_;
  T* end_;
};

using StringView = Span<const char>;

} // namespace mysty
