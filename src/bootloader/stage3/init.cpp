#include "init.hpp"

#include "disk.hpp"
#include "fat.hpp"
#include "globaleventqueue.hpp"
#include "hid/keyboard.hpp"
#include "hid/ps2.hpp"
#include "interrupts.hpp"
#include "memory.hpp"
#include "mysty/io.hpp"
#include "traps.hpp"

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

  constexpr mysty::StringView eventQueueMessage{"  Event Queue...\n"};
  mysty::puts(eventQueueMessage);
  initializeEventQueue();
  return false;

  constexpr mysty::StringView interruptsMessage{"  Interrupts...\n"};
  mysty::puts(interruptsMessage);
  simpleos::initializeInterrupts();

  constexpr mysty::StringView trapsMessage{"  Fault handling...\n"};
  mysty::puts(trapsMessage);
  registerTraps();

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

  constexpr mysty::StringView ps2Message{"  PS/2 Controller...\n"};
  mysty::puts(ps2Message);
  if (!hid::initializePS2Driver()) {
    constexpr mysty::StringView errorMessage{
        "Failed to initialise PS/2 Controller\n"};
    mysty::puts(errorMessage);
  }

  constexpr mysty::StringView keyboardMessage{"  Keyboard...\n"};
  mysty::puts(keyboardMessage);
  if (!hid::initializeKeyboard()) {
    constexpr mysty::StringView errorMessage{"Failed to initialise keyboard\n"};
    mysty::puts(errorMessage);
  }

  constexpr mysty::StringView completeMessage{"Initialisation complete\n"};
  mysty::puts(completeMessage);

  return true;
}

} // namespace simpleos
