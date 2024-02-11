#pragma once

using uint8_t = unsigned char;
using uint16_t = unsigned short;
using uint32_t = unsigned long;
using uint64_t = unsigned long long;

using int8_t = char;
using int16_t = short;
using int32_t = long;
using int64_t = long long;

#ifdef __INTELLISENSE__
using size_t = unsigned int;
#elif defined __clang__
using size_t = unsigned int;
#elif defined __SIZE_TYPE__
using size_t = __SIZE_TYPE__;
#else
using size_t = unsigned int;
#endif

template <typename T>
struct int_limits;

template <>
struct int_limits<uint8_t> {
  static constexpr uint8_t min = 0;
  static constexpr uint8_t max = 0xFF;
};

template <>
struct int_limits<uint16_t> {
  static constexpr uint16_t min = 0;
  static constexpr uint16_t max = 0xFFFF;
};

template <>
struct int_limits<unsigned long> {
  static constexpr unsigned long min = 0;
  static constexpr unsigned long max = 0xFFFFFFFF;
};

template <>
struct int_limits<unsigned int> {
  static constexpr unsigned int min = 0;
  static constexpr unsigned int max = 0xFFFFFFFF;
};

template <>
struct int_limits<uint64_t> {
  static constexpr uint64_t min = 0;
  static constexpr uint64_t max = 0xFFFFFFFFFFFFFFFF;
};
