#include "main.hpp"

#include "disk.hpp"
#include "mystyint.hpp"
#include "mystyio.hpp"

extern "C" {

void __attribute__((cdecl)) cstart(uint8_t bootDrive) {
  mysty::clrscr();
  mysty::puts("Hello, from cstart!\n");
  if (!simpleos::disk::initialize(bootDrive)) {
    mysty::printf(
        "Failed to initialize floppy driver using disk %u\n", bootDrive);
    return;
  }
  mysty::printf(
      "Percent: %%\nChar: %c\nInt: %u\nString: %s\n", 'c', 543, "test string");
}
}
