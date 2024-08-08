#pragma once

#include "mysty/int.hpp"
#include "mysty/storage.hpp"
#include "mysty/typeattributes.hpp"
#include "mysty/utility.hpp"
#include "mysty/vectorsettings.hpp"

namespace mysty {

template <typename T>
class Vector {
 public:
  Vector() : size_(0), capacity_(0), data_(nullptr) {}

  Vector(const Vector& other) : size_(other.size_), capacity_(other.size_) {
    data_ = new StorageFor<T>[capacity_];
    for (size_t i = 0; i < other.size_; i++) {
      data_[i].emplace(other[i]);
    }
  }

  Vector(Vector&& other)
      : size_(other.size_), capacity_(other.capacity_), data_(other.data_) {
    other.size_ = 0;
    other.capacity_ = 0;
    other.data_ = nullptr;
  }

  Vector& operator=(const Vector& other) {
    clear();
    if (capacity_ < other.size_) {
      delete[] data_;
      data_ = new StorageFor<T>[other.size_];
      capacity_ = other.size_;
    }

    for (size_t i = 0; i < other.size_; i++) {
      data_[i].emplace(other[i]);
    }
    size_ = other.size_;
  }

  Vector& operator=(Vector&& other) {
    clear();
    if (data_ != nullptr) {
      delete[] data_;
    }
    size_ = other.size_;
    capacity_ = other.capacity_;
    data_ = other.data_;

    other.size_ = 0;
    other.capacity_ = 0;
    other.data_ = nullptr;
  }

  ~Vector() {
    clear();
    if (data_ != nullptr) {
      delete[] data_;
    }
  }

  void clear() {
    if constexpr (non_trivially_destructible<T>) {
      for (size_t i = 0; i < size_; i++) {
        data_[i].destroy();
      }
    }
    size_ = 0;
  }

  size_t size() const { return size_; }

  bool empty() const { return size_ == 0; }

  T& operator[](size_t i) { return *data_[i]; }
  const T& operator[](size_t i) const { return *data_[i]; }

  template <typename... Args>
  void emplace_back(Args... args) {
    growToFit(size_ + 1);
    data_[size_].emplace(mysty::forward<Args>(args)...);
    size_++;
  }

  void reserve(size_t requiredCapacity) { growToFit(requiredCapacity); }

  T* get() { return data_[0].get(); }
  const T* get() const { return data_[0].get(); }

 private:
  void growToFit(size_t requiredCapacity) {
    if (capacity_ >= requiredCapacity) {
      return;
    }

    size_t newCapacity =
        (capacity_ * kGrowthFactorNumerator) / kGrowthFactorDenominator;
    if (requiredCapacity > newCapacity) {
      newCapacity = requiredCapacity;
    }

    StorageFor<T>* newData_ = new StorageFor<T>[newCapacity];
    for (size_t i = 0; i < size_; i++) {
      newData_[i].emplace(mysty::move(*data_[i]));
      if constexpr (non_trivially_destructible<T>) {
        data_[i].destroy();
      }
    }

    capacity_ = newCapacity;
    delete[] data_;
    data_ = newData_;
  }

  size_t size_;
  size_t capacity_;
  StorageFor<T>* data_;
};

} // namespace mysty
