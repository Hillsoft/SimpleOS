#pragma once

#include "mysty/storage.hpp"
#include "mysty/typeattributes.hpp"
#include "mysty/utility.hpp"

namespace mysty {

template <typename T>
class Optional {
 public:
  constexpr Optional() : hasValue_(false) {}

  template <typename... Args>
  constexpr Optional(Args... args)
      : hasValue_(true), value_(mysty::forward<Args...>(args...)) {}

  Optional(Optional const& other) : hasValue_(other.hasValue()) {
    if (hasValue_) {
      value_.emplace(other.value());
    }
  }

  Optional(Optional&& other) : hasValue_(other.hasValue()) {
    if (hasValue_) {
      value_.emplace(mysty::move(other.value()));
    }
  }

  Optional& operator=(Optional const& other) {
    if (hasValue_ && other.hasValue_) {
      *value_ = *other.value_;
    } else if (hasValue_ && !other.hasValue_) {
      if constexpr (non_trivially_destructible<T>) {
        value_.destroy();
      }
    } else if (!hasValue_ && other.hasValue_) {
      hasValue_ = true;
      value_.emplace(*other.value_);
    } else {
      // Do nothing, we're already both empty
    }
    return *this;
  }

  Optional& operator=(Optional&& other) {
    if (hasValue_ && other.hasValue_) {
      *value_ = mysty::move(*other.value_);
    } else if (hasValue_ && !other.hasValue_) {
      if constexpr (non_trivially_destructible<T>) {
        value_.destroy();
      }
    } else if (!hasValue_ && other.hasValue_) {
      hasValue_ = true;
      value_.emplace(mysty::move(*other.value_));
    } else {
      // Do nothing, we're already both empty
    }
    return *this;
  }

  constexpr ~Optional()
    requires(non_trivially_destructible<T>)
  {
    if (hasValue_) {
      value_.destroy();
    }
  }

  constexpr ~Optional()
    requires(trivially_destructible<T>)
  = default;

  constexpr bool has_value() const { return hasValue_; }

  constexpr T& value() { return *value_; }
  constexpr T const& value() const { return *value_; }

  constexpr T& operator*() { return *value_; }
  constexpr T const& operator*() const { return *value_; }

  constexpr T* operator->() { return &*value_; }
  constexpr T const* operator->() const { return &*value_; }

 private:
  bool hasValue_;
  StorageFor<T> value_;
};

} // namespace mysty
