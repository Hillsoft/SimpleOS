#include "main.hpp"

#include "disk.hpp"
#include "interrupts.hpp"
#include "mysty/array.hpp"
#include "mysty/int.hpp"
#include "mysty/io.hpp"

extern "C" {

void __attribute__((cdecl)) cstart(uint8_t bootDrive) {
  mysty::clrscr();
  mysty::puts("Hello, from cstart!\n");

  simpleos::initializeInterrupts();

  if (!simpleos::disk::initialize(bootDrive)) {
    mysty::printf(
        "Failed to initialize floppy driver using disk %u\n", bootDrive);
    return;
  }
  mysty::printf(
      "Percent: %%\nChar: %c\nInt: %u\nString: %s\n", 'c', 543, "test string");

  mysty::FixedArray<uint8_t, 512> readBuffer;
  if (!simpleos::disk::read(0, readBuffer)) {
    mysty::printf("Failed to read from floppy\n");
  }
  for (size_t i = 0; i < 8; i++) {
    mysty::putc(static_cast<char>(readBuffer[3 + i]));
  }
  mysty::putc('\n');
}
}
