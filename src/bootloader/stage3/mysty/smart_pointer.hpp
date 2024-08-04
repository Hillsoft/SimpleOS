#pragma once

#include "mysty/typeattributes.hpp"
#include "mysty/utility.hpp"

namespace mysty {

template <typename T>
class unique_ptr {
 public:
  explicit unique_ptr() : ptr_(nullptr) {}
  explicit unique_ptr(T* ptr) : ptr_(ptr) {}

  unique_ptr(const unique_ptr& other) = delete;
  unique_ptr& operator=(const unique_ptr& other) = delete;

  unique_ptr(unique_ptr&& other) : ptr_(other.release()) {}
  unique_ptr& operator=(unique_ptr&& other) {
    if (ptr_ != nullptr) {
      delete ptr_;
    }
    ptr_ = other.relase();
  }

  template <typename U>
    requires(mysty::is_base_of<T, U>)
  unique_ptr(unique_ptr<U>&& other) : ptr_(other.release()) {}

  ~unique_ptr() {
    if (ptr_ != nullptr) {
      delete ptr_;
    }
  }

  T* release() {
    T* oldPtr = ptr_;
    ptr_ = nullptr;
    return oldPtr;
  }

  T* get() const { return ptr_; }

  T& operator*() const& { return *ptr_; }
  T&& operator*() && { return mysty::move(*ptr_); }

  T* operator->() const& { return ptr_; }

 private:
  T* ptr_;
};

template <typename T, typename... Args>
unique_ptr<T> make_unique(Args... args) {
  T* obj = new T(mysty::forward<Args...>(args)...);
  return unique_ptr<T>{obj};
}

} // namespace mysty
