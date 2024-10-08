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
concept same_as = is_same_t<T, U>::value;

template <typename T, typename U>
struct is_convertible_t {
  constexpr static bool value = __is_convertible(T, U);
};

template <typename T>
using is_const_t = is_same_t<T, const T>;

template <typename T>
concept is_const = is_const_t<T>::value;

template <typename T>
concept is_not_const = !is_const_t<T>::value;

template <typename T>
struct remove_const {
  using type = T;
};

template <typename T>
struct remove_const<const T> {
  using type = T;
};

template <typename T>
using remove_const_t = remove_const<T>::type;

template <typename T>
struct remove_reference {
  typedef T type;
};

template <typename T>
struct remove_reference<T&> {
  typedef T type;
};

template <typename T>
struct remove_reference<T&&> {
  typedef T type;
};

#ifdef __INTELLISENSE__
template <typename T>
concept trivially_destructible = __is_trivially_destructible(T);
#elif defined __clang__
template <typename T>
concept trivially_destructible = __is_trivially_destructible(T);
#else
template <typename T>
concept trivially_destructible = __has_trivial_destructor(T);
#endif

template <typename T>
concept non_trivially_destructible = !trivially_destructible<T>;

template <typename T, typename U>
concept is_base_of = __is_base_of(T, U);

template <typename T>
struct is_integral_t {
  constexpr static bool value = false;
};

template <>
struct is_integral_t<bool> {
  constexpr static bool value = true;
};

template <>
struct is_integral_t<char> {
  constexpr static bool value = true;
};

template <>
struct is_integral_t<unsigned char> {
  constexpr static bool value = true;
};

template <>
struct is_integral_t<signed char> {
  constexpr static bool value = true;
};

template <>
struct is_integral_t<signed short> {
  constexpr static bool value = true;
};

template <>
struct is_integral_t<unsigned short> {
  constexpr static bool value = true;
};

template <>
struct is_integral_t<signed int> {
  constexpr static bool value = true;
};

template <>
struct is_integral_t<unsigned int> {
  constexpr static bool value = true;
};

template <>
struct is_integral_t<signed long> {
  constexpr static bool value = true;
};

template <>
struct is_integral_t<unsigned long> {
  constexpr static bool value = true;
};

template <>
struct is_integral_t<signed long long> {
  constexpr static bool value = true;
};

template <>
struct is_integral_t<unsigned long long> {
  constexpr static bool value = true;
};

template <typename T>
concept integral = is_integral_t<T>::value;

template <typename T>
struct is_pointer_t {
  constexpr static bool value = false;
};

template <typename T>
struct is_pointer_t<T*> {
  constexpr static bool value = true;
};

template <typename T>
concept pointer = is_pointer_t<T>::value;

} // namespace mysty
