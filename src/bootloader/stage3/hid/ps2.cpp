#include "hid/ps2.hpp"

#include "mysty/int.hpp"
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
  return false;
}

} // namespace simpleos::hid
