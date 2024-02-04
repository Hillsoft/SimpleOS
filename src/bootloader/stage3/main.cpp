#include "main.hpp"
#include "mystyint.hpp"
#include "mystyio.hpp"

extern "C" {

void __attribute__((cdecl)) cstart(uint8_t bootDrive) {
  mysty::clrscr();
  mysty::puts("Hello, from cstart!\n");
  mysty::printf("Percent: %%\nChar: %c\nInt: %u\nString: %s\n", 'c', 543, "test string");
}

}
