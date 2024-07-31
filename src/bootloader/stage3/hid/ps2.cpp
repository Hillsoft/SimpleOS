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

enum class BufferStatus { Empty, Full };

enum PS2Command : uint8_t {
  READ_CONTROLLER_CONFIGURAITON = 0x20,
  WRITE_CONTROLLER_CONFIGURATION = 0x60,
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

  bool firstPortTranslation() const { return getBit(6); }

  void setFirstPortTranslation(bool translationEnabled) {
    setBit(6, translationEnabled);
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

struct StatusRegister {
 public:
  StatusRegister(uint8_t rawByte) : data_(rawByte) {}

  BufferStatus outputBuffer() const {
    if ((data_ & 0b1) > 0) {
      return BufferStatus::Full;
    } else {
      return BufferStatus::Empty;
    }
  }

  BufferStatus inputBuffer() const {
    if (((data_ >> 1) & 0b1) > 0) {
      return BufferStatus::Full;
    } else {
      return BufferStatus::Empty;
    }
  }

  bool systemFlag() const { return (data_ >> 2) & 0b1; }

  bool isCommand() const { return (data_ >> 3) & 0b1; }

  bool timeOutError() const { return (data_ >> 6) & 0b1; }

  bool parityError() const { return (data_ >> 7) & 0b1; }

 private:
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

StatusRegister readStatusRegister() {
  return StatusRegister{x86_inb(kStatusPort)};
}

uint8_t readByte() {
  while (readStatusRegister().outputBuffer() == BufferStatus::Empty) {
  }
  return x86_inb(kDataPort);
}

void writeByte(uint8_t byte) {
  while (readStatusRegister().inputBuffer() == BufferStatus::Full) {
  }
  x86_outb(kDataPort, byte);
}

ControllerConfiguration getConfiguration() {
  x86_outb(kCommandPort, PS2Command::READ_CONTROLLER_CONFIGURAITON);
  return ControllerConfiguration{readByte()};
}

void setConfiguration(ControllerConfiguration configuration) {
  x86_outb(kCommandPort, PS2Command::WRITE_CONTROLLER_CONFIGURATION);
}

bool selfTest(ControllerConfiguration expectedConfiguration) {
  x86_outb(kCommandPort, PS2Command::TEST_CONTROLLER);
  uint8_t selfTestResult = readByte();
  if (selfTestResult != 0x55) {
    return false;
  }

  setConfiguration(expectedConfiguration);

  return true;
}

bool checkIsDualChannel() {
  x86_outb(kCommandPort, PS2Command::ENABLE_SECOND_PORT);
  ControllerConfiguration configuration = getConfiguration();
  return !configuration.clockDisabled(Port::Second);
}

void preInitSecondChannel() {
  x86_outb(kCommandPort, PS2Command::DISABLE_SECOND_PORT);
  ControllerConfiguration configuration = getConfiguration();
  configuration.setInterrupt(Port::Second, false);
  configuration.setClockDisabled(Port::Second, false);
  setConfiguration(configuration);
}

bool testPort(Port port) {
  PS2Command command = port == Port::First ? PS2Command::TEST_FIRST_PORT
                                           : PS2Command::TEST_SECOND_PORT;
  x86_outb(kCommandPort, command);

  uint8_t result = readByte();
  return result == 0;
}

void enablePort(Port port) {
  PS2Command command = port == Port::First ? PS2Command::ENABLE_FIRST_PORT
                                           : PS2Command::ENABLE_SECOND_PORT;
  x86_outb(kCommandPort, command);
  ControllerConfiguration configuration = getConfiguration();
  configuration.setInterrupt(port, true);
  setConfiguration(configuration);
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
  x86_inb(kDataPort);

  ControllerConfiguration configuration = getConfiguration();
  configuration.setInterrupt(Port::First, false);
  configuration.setFirstPortTranslation(false);
  configuration.setClockDisabled(Port::First, true);
  setConfiguration(configuration);

  if (!selfTest(configuration)) {
    constexpr mysty::StringView kErrorMessage{
        "PS/2 Controller self-test failed"};
    mysty::puts(kErrorMessage);
    return false;
  }

  bool isDualChannel = checkIsDualChannel();
  if (isDualChannel) {
    preInitSecondChannel();
  }

  bool firstPortValid = testPort(Port::First);
  bool secondPortValid = isDualChannel && testPort(Port::Second);

  if (!firstPortValid && !secondPortValid) {
    constexpr mysty::StringView kErrorMessage{"No working PS/2 ports"};
    mysty::puts(kErrorMessage);
    return false;
  }

  if (firstPortValid) {
    enablePort(Port::First);
  }
  if (secondPortValid) {
    enablePort(Port::Second);
  }

  return false;
}

} // namespace simpleos::hid
