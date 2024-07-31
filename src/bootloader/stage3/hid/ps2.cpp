#include "hid/ps2.hpp"

#include "mysty/int.hpp"
#include "mysty/io.hpp"
#include "mysty/span.hpp"
#include "x86.hpp"

namespace simpleos::hid {

namespace {

constexpr uint16_t kDataPort = 0x60;
constexpr uint16_t kStatusPort = 0x64;
constexpr uint16_t kCommandPort = 0x64;

enum class Port { First, Second };

enum PS2Command : uint8_t {
  DISABLE_SECOND_PORT = 0xA7,
  ENABLE_SECOND_PORT = 0xA8,
  TEST_SECOND_PORT = 0xA9,
  TEST_CONTROLLER = 0xAA,
  TEST_FIRST_PORT = 0xAB,
  DISABLE_FIRST_PORT = 0xAD,
  ENABLE_FIRST_PORT = 0xAE,
};

struct ControllerConfiguration {
 public:
  explicit ControllerConfiguration(uint8_t data) : data_(data) {}

  uint8_t getData() const { return data_; }

  bool interruptsEnabled(Port port) const {
    switch (port) {
      case Port::First:
        return getBit(0);
      case Port::Second:
        return getBit(1);
      default:
        return false;
    }
  }

  void setInterrupt(Port port, bool interruptsEnabled) {
    switch (port) {
      case Port::First:
        setBit(0, interruptsEnabled);
        break;
      case Port::Second:
        setBit(1, interruptsEnabled);
        break;
      default:
        break;
    }
  }

  bool systemFlag() const { return getBit(2); }

  bool clockDisabled(Port port) const {
    switch (port) {
      case Port::First:
        return getBit(4);
      case Port::Second:
        return getBit(5);
      default:
        return false;
    }
  }

  void setClockDisabled(Port port, bool clockDisabled) {
    switch (port) {
      case Port::First:
        setBit(4, clockDisabled);
        break;
      case Port::Second:
        setBit(5, clockDisabled);
        break;
      default:
        break;
    }
  }

 private:
  void setBit(uint8_t bit, bool value) {
    uint8_t newBit = value ? 1 : 0;
    newBit <<= bit;

    uint8_t bitMask = ~(1 << bit);

    data_ = (data_ & bitMask) | newBit;
  }

  bool getBit(uint8_t bit) const { return ((data_ >> bit) & 0b1) > 0; }

  uint8_t data_;
};

bool doesPS2ControllerExist() {
  // TODO: implement
  return true;
}

void disablePort(Port port) {
  if (port == Port::First) {
    x86_outb(kCommandPort, PS2Command::DISABLE_FIRST_PORT);
  } else {
    x86_outb(kCommandPort, PS2Command::DISABLE_SECOND_PORT);
  }
}

uint8_t readByte() {
  return x86_inb(kDataPort);
}

} // namespace

bool initializePS2Driver() {
  if (!doesPS2ControllerExist()) {
    constexpr mysty::StringView kErrorMessage{"No PS/2 Controller exists"};
    mysty::puts(kErrorMessage);
    return false;
  }

  disablePort(Port::First);
  disablePort(Port::Second);

  // Flush ouptutBuffer
  readByte();

  return false;
}

} // namespace simpleos::hid
