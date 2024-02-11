#pragma once

namespace mysty {

template <typename T>
class Optional {
 public:
  constexpr Optional() : hasValue_(false) {}

  template <typename... Args>
  constexpr Optional(Args... args) : hasValue_(true), value_(args...) {}

  // TODO: implement later
  Optional(Optional const& other) = delete;
  Optional(Optional&& other) = delete;

  Optional& operator=(Optional const& other) = delete;
  Optional& operator=(Optional&& other) = delete;

  constexpr ~Optional() {
    if (hasValue_) {
      value_.~T();
    }
  }

  constexpr bool has_value() const { return hasValue_; }

  constexpr T& value() { return value_; }
  constexpr T const& value() const { return value_; }

  constexpr T& operator*() { return value_; }
  constexpr T const& operator*() const { return value_; }

  constexpr T* operator->() { return &value_; }
  constexpr T const* operator->() const { return &value_; }

 private:
  bool hasValue_;
  union {
    T value_;
  };
};

} // namespace mysty
