#pragma once

#include "mysty/typeattributes.hpp"

namespace mysty {

template <typename T>
concept HasEquality = requires(T x, T y) {
  { x == y } -> same_as<bool>;
  { x != y } -> same_as<bool>;
};

template <typename T>
concept Ordered = requires(T x, T y) {
  requires HasEquality<T>;
  { x < y } -> same_as<bool>;
  { x <= y } -> same_as<bool>;
  { x > y } -> same_as<bool>;
  { x >= y } -> same_as<bool>;
};

template <typename T>
  requires(Ordered<T>)
T const& min(T const& x, T const& y) {
  return x < y ? x : y;
}

template <typename T>
  requires(Ordered<T>)
T const& max(T const& x, T const& y) {
  return x > y ? x : y;
}

} // namespace mysty
