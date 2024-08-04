#include "hid/keyboard.hpp"

#include "hid/ps2.hpp"
#include "mysty/array.hpp"
#include "mysty/int.hpp"
#include "mysty/io.hpp"
#include "mysty/optional.hpp"

namespace simpleos::hid {

namespace {

KeyCode decodeKey(uint8_t rawKeyCode) {
  switch (rawKeyCode) {
    case 0x01:
      return KeyCode::F9;
    case 0x03:
      return KeyCode::F5;
    case 0x04:
      return KeyCode::F3;
    case 0x05:
      return KeyCode::F1;
    case 0x06:
      return KeyCode::F2;
    case 0x07:
      return KeyCode::F12;
    case 0x09:
      return KeyCode::F10;
    case 0x0A:
      return KeyCode::F8;
    case 0x0B:
      return KeyCode::F6;
    case 0x0C:
      return KeyCode::F4;
    case 0x0D:
      return KeyCode::TAB;
    case 0x0E:
      return KeyCode::BACK_TICK;
    case 0x11:
      return KeyCode::LEFT_ALT;
    case 0x12:
      return KeyCode::LEFT_SHIFT;
    case 0x14:
      return KeyCode::LEFT_CONTROL;
    case 0x15:
      return KeyCode::Q;
    case 0x16:
      return KeyCode::NUM1;
    case 0x1A:
      return KeyCode::Z;
    case 0x1B:
      return KeyCode::S;
    case 0x1C:
      return KeyCode::A;
    case 0x1D:
      return KeyCode::W;
    case 0x1E:
      return KeyCode::NUM2;
    case 0x21:
      return KeyCode::C;
    case 0x22:
      return KeyCode::X;
    case 0x23:
      return KeyCode::D;
    case 0x24:
      return KeyCode::E;
    case 0x25:
      return KeyCode::NUM4;
    case 0x26:
      return KeyCode::NUM3;
    case 0x29:
      return KeyCode::SPACE;
    case 0x2A:
      return KeyCode::V;
    case 0x2B:
      return KeyCode::F;
    case 0x2C:
      return KeyCode::T;
    case 0x2D:
      return KeyCode::R;
    case 0x2E:
      return KeyCode::NUM5;
    case 0x31:
      return KeyCode::N;
    case 0x32:
      return KeyCode::B;
    case 0x33:
      return KeyCode::H;
    case 0x34:
      return KeyCode::G;
    case 0x35:
      return KeyCode::Y;
    case 0x36:
      return KeyCode::NUM6;
    case 0x3A:
      return KeyCode::M;
    case 0x3B:
      return KeyCode::J;
    case 0x3C:
      return KeyCode::U;
    case 0x3D:
      return KeyCode::NUM7;
    case 0x38:
      return KeyCode::NUM8;
    case 0x41:
      return KeyCode::COMMA;
    case 0x42:
      return KeyCode::K;
    case 0x43:
      return KeyCode::I;
    case 0x44:
      return KeyCode::O;
    case 0x45:
      return KeyCode::NUM0;
    case 0x46:
      return KeyCode::NUM9;
    case 0x49:
      return KeyCode::PERIOD;
    case 0x4A:
      return KeyCode::FORWARD_SLASH;
    case 0x4B:
      return KeyCode::L;
    case 0x4C:
      return KeyCode::SEMICOLON;
    case 0x4D:
      return KeyCode::P;
    case 0x4E:
      return KeyCode::DASH;
    case 0x52:
      return KeyCode::APOSTROPHE;
    case 0x54:
      return KeyCode::OPEN_SQUARE_BRACKET;
    case 0x55:
      return KeyCode::EQUAL;
    case 0x58:
      return KeyCode::CAPS_LOCK;
    case 0x59:
      return KeyCode::RIGHT_SHIFT;
    case 0x5A:
      return KeyCode::ENTER;
    case 0x5B:
      return KeyCode::CLOSE_SQUARE_BRACKET;
    case 0x5D:
      return KeyCode::HASH;
    case 0x61:
      return KeyCode::BACK_SLASH;
    case 0x66:
      return KeyCode::BACKSPACE;
    case 0x69:
      return KeyCode::KEYPAD1;
    case 0x6B:
      return KeyCode::KEYPAD4;
    case 0x6C:
      return KeyCode::KEYPAD7;
    case 0x70:
      return KeyCode::KEYPAD0;
    case 0x71:
      return KeyCode::KEYPAD_DOT;
    case 0x72:
      return KeyCode::KEYPAD2;
    case 0x73:
      return KeyCode::KEYPAD5;
    case 0x74:
      return KeyCode::KEYPAD6;
    case 0x75:
      return KeyCode::KEYPAD8;
    case 0x76:
      return KeyCode::ESCAPE;
    case 0x77:
      return KeyCode::NUM_LOCK;
    case 0x78:
      return KeyCode::F11;
    case 0x79:
      return KeyCode::KEYPAD_PLUS;
    case 0x7A:
      return KeyCode::KEYPAD3;
    case 0x7B:
      return KeyCode::KEYPAD_MINUS;
    case 0x7C:
      return KeyCode::KEYPAD_MULTIPLY;
    case 0x7D:
      return KeyCode::KEYPAD9;
    case 0x7E:
      return KeyCode::SCROLL_LOCK;
    case 0x83:
      return KeyCode::F7;
    default:
      return KeyCode::UNKNOWN;
  };
}

char asciiDecode(KeyCode keyCode) {
  switch (keyCode) {
    case KeyCode::TAB:
      return '\t';
    case KeyCode::BACK_TICK:
      return '`';
    case KeyCode::NUM0:
      return '0';
    case KeyCode::NUM1:
      return '1';
    case KeyCode::NUM2:
      return '2';
    case KeyCode::NUM3:
      return '3';
    case KeyCode::NUM4:
      return '4';
    case KeyCode::NUM5:
      return '5';
    case KeyCode::NUM6:
      return '6';
    case KeyCode::NUM7:
      return '7';
    case KeyCode::NUM8:
      return '8';
    case KeyCode::NUM9:
      return '9';
    case KeyCode::KEYPAD0:
      return '0';
    case KeyCode::KEYPAD1:
      return '1';
    case KeyCode::KEYPAD2:
      return '2';
    case KeyCode::KEYPAD3:
      return '3';
    case KeyCode::KEYPAD4:
      return '4';
    case KeyCode::KEYPAD5:
      return '5';
    case KeyCode::KEYPAD6:
      return '6';
    case KeyCode::KEYPAD7:
      return '7';
    case KeyCode::KEYPAD8:
      return '8';
    case KeyCode::KEYPAD9:
      return '9';
    case KeyCode::KEYPAD_DOT:
      return '.';
    case KeyCode::KEYPAD_PLUS:
      return '+';
    case KeyCode::KEYPAD_MINUS:
      return '-';
    case KeyCode::KEYPAD_MULTIPLY:
      return '*';
    case KeyCode::SPACE:
      return ' ';
    case KeyCode::ENTER:
      return '\n';
    case KeyCode::PERIOD:
      return '.';
    case KeyCode::COMMA:
      return ',';
    case KeyCode::SEMICOLON:
      return ';';
    case KeyCode::DASH:
      return '-';
    case KeyCode::FORWARD_SLASH:
      return '/';
    case KeyCode::BACK_SLASH:
      return '\\';
    case KeyCode::HASH:
      return '#';
    case KeyCode::EQUAL:
      return '=';
    case KeyCode::APOSTROPHE:
      return '\'';
    case KeyCode::OPEN_SQUARE_BRACKET:
      return '[';
    case KeyCode::CLOSE_SQUARE_BRACKET:
      return ']';
    case KeyCode::A:
      return 'a';
    case KeyCode::B:
      return 'b';
    case KeyCode::C:
      return 'c';
    case KeyCode::D:
      return 'd';
    case KeyCode::E:
      return 'e';
    case KeyCode::F:
      return 'f';
    case KeyCode::G:
      return 'g';
    case KeyCode::H:
      return 'h';
    case KeyCode::I:
      return 'i';
    case KeyCode::J:
      return 'j';
    case KeyCode::K:
      return 'k';
    case KeyCode::L:
      return 'l';
    case KeyCode::M:
      return 'm';
    case KeyCode::N:
      return 'n';
    case KeyCode::O:
      return 'o';
    case KeyCode::P:
      return 'p';
    case KeyCode::Q:
      return 'q';
    case KeyCode::R:
      return 'r';
    case KeyCode::S:
      return 's';
    case KeyCode::T:
      return 't';
    case KeyCode::U:
      return 'u';
    case KeyCode::V:
      return 'v';
    case KeyCode::W:
      return 'w';
    case KeyCode::X:
      return 'x';
    case KeyCode::Y:
      return 'y';
    case KeyCode::Z:
      return 'z';
    default:
      return 0;
  }
}

mysty::Optional<PS2PortHandle> keyboardPortHandle;

void keyboardInputHandler() {
  // We assume keyboardPortHandle is not empty, this will never be registered
  // otherwise

  mysty::FixedCircularBuffer<uint8_t, 64U>& buffer =
      getDeviceBuffer(*keyboardPortHandle);

  while (buffer.size() > 0) {
    uint8_t rawByte = buffer.pop_front();
    char currentCharacter = asciiDecode(decodeKey(rawByte));
    if (currentCharacter > 0) {
      mysty::printf("%X: %c\n", rawByte, currentCharacter);
    } else {
      mysty::printf("%X: unprintable\n", rawByte);
    }
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
