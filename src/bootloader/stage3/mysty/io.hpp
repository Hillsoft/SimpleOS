#pragma once

#include "mysty/typeattributes.hpp"

namespace mysty {

void clrscr();
void putc(char c);
void puts(const char* str);
void putuint(unsigned int i, unsigned int base = 10);
void putint(int i, unsigned int base = 10);

inline void printf(const char* format) {
  while (*format != 0) {
    if (*format != '%') {
      putc(*format);
      format++;
    } else {
      puts("<empty format data>");
    }
  }
}

template <typename Arg, typename... Args>
void printf(const char* format, Arg first, Args... args) {
  while (*format != 0) {
    if (*format != '%') {
      putc(*format);
      format++;
    } else {
      // Format specifier
      format += 1;

      switch (*format) {
        case 0:
          return;

        case '%':
          putc('%');
          break;

        case 'u':
          if constexpr (is_convertible_t<Arg, unsigned int>::value) {
            putuint(static_cast<unsigned int>(first));
          } else {
            puts("<mismatched type in format>");
          }
          printf(format + 1, args...);
          return;

        case 'd':
        case 'i':
          if constexpr (is_convertible_t<Arg, int>::value) {
            putint(static_cast<int>(first));
          } else {
            puts("<mismatched type in format>");
          }
          printf(format + 1, args...);
          return;

        case 'o':
          if constexpr (is_convertible_t<Arg, int>::value) {
            putint(static_cast<int>(first), 8);
          } else {
            puts("<mismatched type in format>");
          }
          printf(format + 1, args...);
          return;

        case 'X':
          if constexpr (is_convertible_t<Arg, unsigned int>::value) {
            putuint(static_cast<unsigned int>(first), 16);
          } else {
            puts("<mismatched type in format>");
          }
          printf(format + 1, args...);
          return;

        case 'c':
          if constexpr (is_convertible_t<Arg, char>::value) {
            putc(static_cast<char>(first));
          } else {
            puts("<mismatched type in format>");
          }
          printf(format + 1, args...);
          return;

        case 's':
          if constexpr (is_convertible_t<Arg, const char*>::value) {
            puts(static_cast<const char*>(first));
          } else {
            puts("<mismatched type in format>");
          }
          printf(format + 1, args...);
          return;

        default:
          puts("<bad specifier>");
          break;
      }

      format += 1;
    }
  }
}

} // namespace mysty
