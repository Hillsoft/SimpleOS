#pragma once

#include "mysty/int.hpp"

namespace mysty {

constexpr size_t strlen(const char* str) noexcept {
  size_t len = 0;
  while (str[len] != 0) {
    ++len;
  }
  return len;
}

class String {
 public:
  String(const char* str);

  String(String const& other);
  String(String&& other);

  String& operator=(String const& other);
  String& operator=(String&& other);

  ~String();

  size_t size() const;
  char* get();
  const char* get() const;

 private:
  size_t size_;
  size_t capacity_;
  char* buffer_;
};

} // namespace mysty
