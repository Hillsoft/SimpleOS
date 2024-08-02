#include "main.hpp"

#include "fat.hpp"
#include "init.hpp"
#include "mysty/array.hpp"
#include "mysty/int.hpp"
#include "mysty/io.hpp"
#include "mysty/string.hpp"

namespace {

void testFile() {
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

void testMemory() {
  uint8_t* ptra = new uint8_t[256];

  mysty::printf("ptra: %X\n", reinterpret_cast<size_t>(ptra));

  uint8_t* ptrb = new uint8_t[256];

  mysty::printf("ptrb: %X\n", reinterpret_cast<size_t>(ptrb));

  delete[] ptra;

  uint8_t* ptrc = new uint8_t[5];

  mysty::printf("ptrc: %X\n", reinterpret_cast<size_t>(ptrc));

  uint8_t* ptrd = new uint8_t[5];

  mysty::printf("ptrd: %X\n", reinterpret_cast<size_t>(ptrd));

  delete[] ptrb;
  delete[] ptrc;
  delete[] ptrd;

  uint8_t* ptre = new uint8_t[512];

  mysty::printf("ptre: %X\n", reinterpret_cast<size_t>(ptre));

  delete[] ptre;
}

void testString() {
  mysty::String myStr = "Test string\n";
  mysty::puts(myStr);
}

} // namespace

extern "C" {

void __attribute__((cdecl)) __attribute__((externally_visible))
cstart(uint8_t bootDrive) {
  if (!simpleos::initialize(bootDrive)) {
    constexpr mysty::StringView errorMessage{
        "Failed to initialise, aborting!\n"};
    mysty::puts(errorMessage);
    return;
  }

  // testFile();
  // testMemory();
  // testString();
}
}
