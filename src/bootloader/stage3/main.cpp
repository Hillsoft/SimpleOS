#include "main.hpp"
#include "mystyint.hpp"
#include "mystyio.hpp"

extern "C" {

void __attribute__((cdecl)) cstart(uint8_t bootDrive) {
  mysty::clrscr();
  mysty::puts("Hello, from cstart!\n");
  mysty::putuint(2941);
  mysty::putuint(2941, 16);
}

}
