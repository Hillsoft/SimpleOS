#pragma once

namespace mysty {

template <typename T>
class Optional {
 public:
  Optional() : hasValue_(false) {}

  template <typename... Args>
  Optional(Args... args) : hasValue_(true), value_(args...) {}

  // TODO: implement later
  Optional(Optional const& other) = delete;
  Optional(Optional&& other) = delete;

  Optional& operator=(Optional const& other) = delete;
  Optional& operator=(Optional&& other) = delete;

  ~Optional() {
    if (hasValue_) {
      value_.~T();
    }
  }

  bool has_value() const { return hasValue_; }

  T& value() { return value_; }
  T const& value() const { return value_; }

  T& operator*() { return value_; }
  T const& operator*() const { return value_; }

  T* operator->() { return &value_; }
  T const* operator->() const { return &value_; }

 private:
  bool hasValue_;
  union {
    T value_;
  };
};

} // namespace mysty
