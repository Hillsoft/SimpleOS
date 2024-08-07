#pragma once

namespace simpleos::hid {

enum class KeyCode {
  UNKNOWN,
  F1,
  F2,
  F3,
  F4,
  F5,
  F6,
  F7,
  F8,
  F9,
  F10,
  F11,
  F12,
  LEFT_ALT,
  LEFT_SHIFT,
  LEFT_CONTROL,
  RIGHT_ALT,
  RIGHT_SHIFT,
  RIGHT_CONTROL,
  NUM_LOCK,
  CAPS_LOCK,
  SCROLL_LOCK,
  ESCAPE,
  BACKSPACE,
  TAB,
  BACK_TICK,
  NUM0,
  NUM1,
  NUM2,
  NUM3,
  NUM4,
  NUM5,
  NUM6,
  NUM7,
  NUM8,
  NUM9,
  KEYPAD0,
  KEYPAD1,
  KEYPAD2,
  KEYPAD3,
  KEYPAD4,
  KEYPAD5,
  KEYPAD6,
  KEYPAD7,
  KEYPAD8,
  KEYPAD9,
  KEYPAD_DOT,
  KEYPAD_PLUS,
  KEYPAD_MINUS,
  KEYPAD_MULTIPLY,
  SPACE,
  ENTER,
  PERIOD,
  COMMA,
  SEMICOLON,
  DASH,
  FORWARD_SLASH,
  BACK_SLASH,
  HASH,
  EQUAL,
  APOSTROPHE,
  OPEN_SQUARE_BRACKET,
  CLOSE_SQUARE_BRACKET,
  A,
  B,
  C,
  D,
  E,
  F,
  G,
  H,
  I,
  J,
  K,
  L,
  M,
  N,
  O,
  P,
  Q,
  R,
  S,
  T,
  U,
  V,
  W,
  X,
  Y,
  Z,
};

struct KeyboardModifierSet {
  bool controlHeld = false;
  bool shiftHeld = false;
  bool altHeld = false;
};

struct KeyboardEvent {
  enum Type {
    PRESS,
    RELEASE,
  };

  Type type;
  KeyCode code;
  char ascii;
  KeyboardModifierSet modifierSet;
};

bool initializeKeyboard();

} // namespace simpleos::hid
