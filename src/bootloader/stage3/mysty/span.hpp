#pragma once

#include "mysty/concepts/iterable.hpp"
#include "mysty/int.hpp"

namespace mysty {

template <typename T>
class Span {
 private:
  class Iterator {
   public:
    using value_t = T;

    Iterator(T* current) : current_(current) {}

    bool operator==(Iterator const& other) const {
      return current_ == other.current_;
    }

    T& operator*() const { return *current_; }

    Iterator const& operator++() {
      current_ += 1;
      return *this;
    }

   protected:
    T* current_;
  };

 public:
  using iterator_t = Iterator;

  Span() : start_(nullptr), end_(nullptr) {}
  Span(T* start, T* end) : start_(start), end_(end) {}

  T& at(size_t i) const { return start_[i]; }
  T& operator[](size_t i) const { return start_[i]; }

  iterator_t begin() const { return iterator_t{start_}; }
  iterator_t end() const { return iterator_t{end_}; }

 protected:
  T* start_;
  T* end_;
};

} // namespace mysty
