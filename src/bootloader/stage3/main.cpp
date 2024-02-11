#include "main.hpp"

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

  mysty::Optional<simpleos::File> testFile = simpleos::openFile("TEST.TXT");
  if (!testFile.has_value()) {
    mysty::puts("Failed to open 'test.txt'\n");
    return;
  }

  mysty::FixedArray<uint8_t, 100> readBuffer;
  simpleos::File::ReadResult result = simpleos::File::ReadResult::OK;

  while (result == simpleos::File::ReadResult::OK) {
    size_t remaining = testFile->remainingBytes();

    result = testFile->read(readBuffer);

    if (result == simpleos::File::ReadResult::FAILED) {
      mysty::puts("\nFailed to read file\n");
      return;
    }

    for (size_t i = 0; i < mysty::min(readBuffer.size(), remaining); i++) {
      mysty::putc(static_cast<char>(readBuffer[i]));
    }
  }

  mysty::putc('\n');
}
}
