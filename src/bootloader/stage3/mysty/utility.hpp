#pragma once

namespace mysty {

template <typename T>
struct Identity {
  using type = T;
};

template <typename T>
constexpr T&& forward(typename Identity<T>::type& x) {
  return static_cast<Identity<T>::type&&>(x);
}

template <typename T>
constexpr T&& forward(typename Identity<T>::type&& x) {
  return static_cast<Identity<T>::type&&>(x);
}

} // namespace mysty
