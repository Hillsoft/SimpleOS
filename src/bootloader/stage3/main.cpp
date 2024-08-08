#include "main.hpp"

#include "globaleventqueue.hpp"
#include "init.hpp"
#include "interrupts.hpp"
#include "mysty/int.hpp"
#include "mysty/io.hpp"
#include "terminal/terminal.hpp"

extern "C" {

void __attribute__((cdecl)) __attribute__((externally_visible)) cstart(
    uint8_t bootDrive) {
  if (!simpleos::initialize(bootDrive)) {
    constexpr mysty::StringView errorMessage{
        "Failed to initialise, aborting!\n"};
    mysty::puts(errorMessage);
    return;
  }

  simpleos::startTerminal();

  while (true) {
    if (simpleos::areEventsWaiting()) {
      simpleos::dispatchNextEvent();
    } else {
      simpleos::awaitInterrupt();
    }
  }
}
}
