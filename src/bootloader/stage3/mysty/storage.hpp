#pragma once

#include "mysty/new.hpp"
#include "mysty/typeattributes.hpp"
#include "mysty/utility.hpp"

namespace mysty {

template <typename T>
class StorageFor {
 public:
  explicit constexpr StorageFor() {}

  template <typename... Args>
  explicit constexpr StorageFor(Args... args) {
    emplace(mysty::forward<Args...>(args)...);
  }

  StorageFor(const StorageFor& other) = delete;
  StorageFor(StorageFor&& other) = delete;

  StorageFor& operator=(const StorageFor& other) = delete;
  StorageFor& operator=(StorageFor&& other) = delete;

  template <typename... Args>
  void emplace(Args... args) {
    new (buffer_) T(mysty::forward<Args>(args)...);
  }

  void destroy()
    requires(non_trivially_destructible<T>)
  {
    get()->~T();
  }

  T* get() { return reinterpret_cast<T*>(buffer_); }
  const T* get() const { return reinterpret_cast<T*>(buffer_); }
  T& operator*() { return *get(); }
  const T& operator*() const { return *get(); }
  T* operator->() { return get(); }
  const T* operator->() const { return get(); }

 private:
  alignas(T) char buffer_[sizeof(T)]{0};
};

} // namespace mysty
