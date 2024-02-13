#include "init.hpp"

#include "disk.hpp"
#include "fat.hpp"
#include "interrupts.hpp"
#include "memory.hpp"
#include "mysty/io.hpp"

namespace simpleos {

bool initialize(uint8_t bootDrive) {
  mysty::clrscr();
  constexpr mysty::StringView initializingMessage{"Initialising SimpleOS...\n"};
  mysty::puts(initializingMessage);

  constexpr mysty::StringView memoryMessage{"  Memory...\n"};
  mysty::puts(memoryMessage);
  if (!simpleos::initializeMemory()) {
    constexpr mysty::StringView errorMessage{"Failed to initialize memory\n"};
    mysty::puts(errorMessage);
    return false;
  }

  constexpr mysty::StringView interruptsMessage{"  Interrupts...\n"};
  mysty::puts(interruptsMessage);
  simpleos::initializeInterrupts();

  constexpr mysty::StringView floppyMessage{"  Floppy Disk Driver...\n"};
  mysty::puts(floppyMessage);
  if (!simpleos::disk::initialize(bootDrive)) {
    mysty::printf(
        "Failed to initialize floppy driver using disk %u\n", bootDrive);
    return false;
  }

  constexpr mysty::StringView fileSystemMessage{"  File system...\n"};
  mysty::puts(fileSystemMessage);
  if (!initializeFileSystem()) {
    constexpr mysty::StringView errorMessage{
        "Failed to initialise file system\n"};
    mysty::puts(errorMessage);
    return false;
  }

  constexpr mysty::StringView completeMessage{"Initialisation complete\n"};
  mysty::puts(completeMessage);

  return true;
}

} // namespace simpleos
