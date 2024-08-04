#pragma once

#include "mysty/typeattributes.hpp"

namespace mysty {

template <typename T>
struct Identity {
  using type = T;
};

template <typename T>
constexpr remove_reference<T>::type&& move(T& obj) {
  return static_cast<remove_reference<T>::type&&>(obj);
}

template <typename T>
constexpr T&& forward(typename Identity<T>::type& x) {
  return static_cast<Identity<T>::type&&>(x);
}

template <typename T>
constexpr T&& forward(typename Identity<T>::type&& x) {
  return static_cast<Identity<T>::type&&>(x);
}

} // namespace mysty
