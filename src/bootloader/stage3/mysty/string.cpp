#include "string.hpp"

#include "mysty/memory.hpp"
#include "mysty/vectorsettings.hpp"

namespace mysty {

String::String() : size_(0), capacity_(1), buffer_(new char[1]) {
  buffer_[0] = 0;
}

String::String(const char* str)
    : size_(strlen(str)), capacity_(size_ + 1), buffer_(new char[capacity_]) {
  mysty::memcpy(buffer_, str, capacity_);
}

String::String(String const& other)
    : size_(other.size_), capacity_(size_ + 1), buffer_(new char[capacity_]) {
  mysty::memcpy(buffer_, other.buffer_, capacity_);
}

String::String(String&& other)
    : size_(other.size_), capacity_(other.capacity_), buffer_(other.buffer_) {
  other.size_ = 0;
  other.capacity_ = 0;
  other.buffer_ = nullptr;
}

String& String::operator=(String const& other) {
  if (&other == this) {
    return *this;
  }

  if (other.size_ + 1 <= capacity_) {
    // we don't need a new buffer
    size_ = other.size_;
    mysty::memcpy(buffer_, other.buffer_, size_ + 1);
  } else {
    delete[] buffer_;

    size_ = other.size_;
    capacity_ = size_ + 1;
    buffer_ = new char[capacity_];
    mysty::memcpy(buffer_, other.buffer_, size_ + 1);
  }

  return *this;
}

String& String::operator=(String&& other) {
  if (&other == this) {
    return *this;
  }

  delete[] buffer_;

  size_ = other.size_;
  capacity_ = other.capacity_;
  buffer_ = other.buffer_;

  other.size_ = 0;
  other.capacity_ = 0;
  other.buffer_ = nullptr;

  return *this;
}

String::~String() {
  delete[] buffer_;
}

size_t String::size() const {
  return size_;
}

char* String::get() {
  return buffer_;
}

const char* String::get() const {
  return buffer_;
}

void String::append(char c) {
  size_t requiredCapacity = size_ + 2;
  if (capacity_ < requiredCapacity) {
    size_t newCapacity =
        (capacity_ * kGrowthFactorNumerator) / kGrowthFactorDenominator;
    if (newCapacity < requiredCapacity) {
      newCapacity = requiredCapacity;
    }

    char* newBuffer = new char[newCapacity];
    mysty::memcpy(newBuffer, buffer_, capacity_);
    delete[] buffer_;
    buffer_ = newBuffer;
    capacity_ = newCapacity;
  }

  buffer_[size_] = c;
  buffer_[size_ + 1] = 0;
  size_++;
}

void String::exchange(String& other) {
  size_t tmpSize = size_;
  size_t tmpCapacity = capacity_;
  char* tmpBuffer = buffer_;

  size_ = other.size_;
  capacity_ = other.capacity_;
  buffer_ = other.buffer_;

  other.size_ = tmpSize;
  other.capacity_ = tmpCapacity;
  other.buffer_ = tmpBuffer;
}

} // namespace mysty
