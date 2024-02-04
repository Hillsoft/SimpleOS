#include "main.hpp"
#include "mystyint.hpp"
#include "mystyio.hpp"

extern "C" {

void __attribute__((cdecl)) cstart(uint8_t bootDrive) {
  mysty::clrscr();
  mysty::puts("Hello, from cstart!\n");
  mysty::puts("Hello 2\n");
  mysty::puts("3\n");
  mysty::puts("4\n");
  mysty::puts("5\n");
  mysty::puts("6\n");
  mysty::puts("7\n");
  mysty::puts("8\n");
  mysty::puts("9\n");
  mysty::puts("10\n");
  mysty::puts("11\n");
  mysty::puts("12\n");
  mysty::puts("13\n");
  mysty::puts("14\n");
  mysty::puts("15\n");
  mysty::puts("16\n");
  mysty::puts("17\n");
  mysty::puts("18\n");
  mysty::puts("19\n");
  mysty::puts("20\n");
  mysty::puts("21\n");
  mysty::puts("22\n");
  mysty::puts("23\n");
  mysty::puts("24\n");
  mysty::puts("25\n");
}

}
