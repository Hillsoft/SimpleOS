#include "main.hpp"

#include "fat.hpp"
#include "init.hpp"
#include "mysty/array.hpp"
#include "mysty/int.hpp"
#include "mysty/io.hpp"

extern "C" {

void __attribute__((cdecl)) cstart(uint8_t bootDrive) {
  if (!simpleos::initialize(bootDrive)) {
    constexpr mysty::StringView errorMessage{
        "Failed to initialise, aborting!\n"};
    mysty::puts(errorMessage);
    return;
  }

  constexpr mysty::StringView testFileName{"TEST.TXT"};
  mysty::Optional<simpleos::File> testFile = simpleos::openFile(testFileName);
  if (!testFile.has_value()) {
    constexpr mysty::StringView errorMessage{"Failed to open 'test.txt'\n"};
    mysty::puts(errorMessage);
    return;
  }

  mysty::FixedArray<uint8_t, 100> readBuffer;
  simpleos::File::ReadResult result = simpleos::File::ReadResult::OK;

  while (result == simpleos::File::ReadResult::OK) {
    size_t remaining = testFile->remainingBytes();

    result = testFile->read(readBuffer);

    if (result == simpleos::File::ReadResult::FAILED) {
      constexpr mysty::StringView errorMessage{"\nFailed to read file\n"};
      mysty::puts(errorMessage);
      return;
    }

    mysty::puts(static_cast<mysty::Span<uint8_t>>(readBuffer)
                    .reinterpret<const char>()
                    .slice_front(0, remaining));
  }

  mysty::putc('\n');
}
}
