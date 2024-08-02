#include "hid/keyboard.hpp"

#include "hid/ps2.hpp"
#include "mysty/array.hpp"
#include "mysty/int.hpp"
#include "mysty/io.hpp"
#include "mysty/optional.hpp"

namespace simpleos::hid {

namespace {

mysty::Optional<PS2PortHandle> keyboardPortHandle;

void keyboardInputHandler() {
  // We assume keyboardPortHandle is not empty, this will never be registered
  // otherwise

  mysty::FixedCircularBuffer<uint8_t, 64U>& buffer =
      getDeviceBuffer(*keyboardPortHandle);

  while (buffer.size() > 0) {
    mysty::printf("%X ", buffer.pop_front());
  }
}

} // namespace

bool initializeKeyboard() {
  keyboardPortHandle = getPortForDevice(PS2DeviceType::MF2KEYBOARD);
  if (!keyboardPortHandle.has_value()) {
    constexpr mysty::StringView kNoKeyboardMessage = "No keyboard found\n";
    mysty::puts(kNoKeyboardMessage);
    return false;
  }
  mysty::printf(
      "Keyboard found on port %d\n",
      static_cast<uint8_t>(keyboardPortHandle->getPort()) + 1);

  // Enable scanning
  sendBytesToDevice(*keyboardPortHandle, mysty::FixedArray<uint8_t, 1>{0xF4});
  mysty::Optional<uint8_t> enableScanningResponse =
      awaitByteFromDevice(*keyboardPortHandle, 10);
  if (!enableScanningResponse.has_value() || *enableScanningResponse != 0xFA) {
    constexpr mysty::StringView kNoScanningMessage =
        "Failed to enable scanning\n";
    mysty::puts(kNoScanningMessage);
    return false;
  }

  registerPS2InputHandler(*keyboardPortHandle, keyboardInputHandler);

  return true;
}

} // namespace simpleos::hid
