#include "main.hpp"

#include "disk.hpp"
#include "fat.hpp"
#include "init.hpp"
#include "mysty/array.hpp"
#include "mysty/int.hpp"
#include "mysty/io.hpp"

extern "C" {

void __attribute__((cdecl)) cstart(uint8_t bootDrive) {
  if (!simpleos::initialize(bootDrive)) {
    mysty::puts("Failed to initialise, aborting!\n");
    return;
  }

  mysty::Optional<simpleos::File> badFile = simpleos::openFile("no.txt");
  if (!badFile.has_value()) {
    mysty::puts("Failed to open 'no.txt'\n");
  }

  mysty::Optional<simpleos::File> testFile = simpleos::openFile("TEST.TXT");
  if (!testFile.has_value()) {
    mysty::puts("Failed to open 'test.txt'\n");
  }

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
