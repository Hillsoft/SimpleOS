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
