#include "mystyint.hpp"
#include "mystyio.hpp"
#include "x86.hpp"

namespace mysty {

namespace {

constexpr int kScreenWidth = 80;
constexpr int kScreenHeight = 25;
constexpr uint8_t kDefaultColor = 0x7;

volatile uint8_t* const g_screenBuffer = reinterpret_cast<uint8_t*>(0xB8000);
int g_screenX = 0;
int g_screenY = 0;

void putchr(int x, int y, char c) {
  g_screenBuffer[2 * (y * kScreenWidth + x)] = static_cast<uint8_t>(c);
}

char getchr(int x, int y) {
  return static_cast<char>(g_screenBuffer[2 * (y * kScreenWidth + x)]);
}

void putcolor(int x, int y, uint8_t color) {
  g_screenBuffer[2 * (y * kScreenWidth + x) + 1] = color;
}

uint8_t getcolor(int x, int y) {
  return g_screenBuffer[2 * (y * kScreenWidth + x) + 1];
}

void scrollback() {
  for (int y = 0; y < kScreenHeight - 1; y++) {
    for (int x = 0; x < kScreenWidth; x++) {
      putchr(x, y, getchr(x, y + 1));
      putcolor(x, y, getcolor(x, y + 1));
    }
  }

  for (int x = 0; x < kScreenWidth; x++) {
    putchr(x, kScreenHeight - 1, ' ');
    putcolor(x, kScreenHeight - 1, kDefaultColor);
  }

  g_screenY = kScreenHeight - 1;
}

void setcursor(int x, int y) {
  int pos = y * kScreenWidth + x;

  x86_outb(0x3D4, 0x0F);
  x86_outb(0x3D5, static_cast<uint8_t>(pos & 0xFF));
  x86_outb(0x3D4, 0x0E);
  x86_outb(0x3D5, static_cast<uint8_t>((pos >> 8) & 0xFF));
}

} // namespace

void clrscr() {
  for (int y = 0; y < kScreenHeight; y++) {
    for (int x = 0; x < kScreenWidth; x++) {
      putchr(x, y, ' ');
      putcolor(x, y, kDefaultColor);
    }
  }
  g_screenX = 0;
  g_screenY = 0;
}

void putc(char c) {
  switch (c) {
    case '\n':
      g_screenX = 0;
      g_screenY++;
      break;

    case '\r':
      g_screenX = 0;
      break;

    case '\t':
      for (int i = 0; i < 4 - (g_screenX % 4); i++) {
        putc(' ');
      }
      break;

    default:
      putchr(g_screenX, g_screenY, c);
      g_screenX++;
  }

  if (g_screenX >= kScreenWidth) {
    g_screenX = 0;
    g_screenY++;
  }

  if (g_screenY >= kScreenHeight) {
    scrollback();
  }

  setcursor(g_screenX, g_screenY);
}

void puts(const char* str) {
  while (*str != 0) {
    putc(*str);
    str++;
  }
}

} // namespace mysty
