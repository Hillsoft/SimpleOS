#include "init.hpp"

#include "disk.hpp"
#include "fat.hpp"
#include "interrupts.hpp"
#include "mysty/io.hpp"

namespace simpleos {

bool initialize(uint8_t bootDrive) {
  mysty::clrscr();
  mysty::puts("Initialising SimpleOS...\n");

  mysty::puts("  Interrupts...\n");
  simpleos::initializeInterrupts();

  mysty::puts("  Floppy Disk Driver...\n");
  if (!simpleos::disk::initialize(bootDrive)) {
    mysty::printf(
        "Failed to initialize floppy driver using disk %u\n", bootDrive);
    return false;
  }

  mysty::puts("  File system...\n");
  if (!initializeFileSystem()) {
    mysty::puts("Failed to initialise file system\n");
    return false;
  }

  mysty::puts("Initialisation complete\n");

  return true;
}

} // namespace simpleos
