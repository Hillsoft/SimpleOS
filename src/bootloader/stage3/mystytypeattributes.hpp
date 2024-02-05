#pragma once

namespace mysty {

template <typename T, typename U>
struct is_same_t {
  constexpr static bool value = false;
};

template <typename T>
struct is_same_t<T, T> {
  constexpr static bool value = true;
};

template <typename T, typename U>
struct is_convertible_t {
  constexpr static bool value = __is_convertible(T, U);
};

} // namespace mysty
