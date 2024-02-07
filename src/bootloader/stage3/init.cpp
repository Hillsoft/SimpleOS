#include "init.hpp"

#include "disk.hpp"
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

  return true;
}

} // namespace simpleos
