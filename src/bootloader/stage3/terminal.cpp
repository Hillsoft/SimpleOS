#include "terminal.hpp"

#include "globaleventqueue.hpp"
#include "hid/keyboard.hpp"
#include "mysty/io.hpp"
#include "mysty/span.hpp"
#include "mysty/storage.hpp"

namespace simpleos {

namespace {

class KeyboardEventHandler : public EventHandler<hid::KeyboardEvent> {
 public:
  void handleEvent(hid::KeyboardEvent event) override {
    if (event.ascii > 0 && event.type == hid::KeyboardEvent::Type::PRESS) {
      mysty::putc(event.ascii);
    }
  }
};

mysty::StorageFor<KeyboardEventHandler> eventHandler;

} // namespace

void startTerminal() {
  mysty::clrscr();
  constexpr mysty::StringView kWelcomeMessage = "Welcome to SimpleOS!\n\n\n";
  mysty::puts(kWelcomeMessage);

  eventHandler.emplace();
  registerHandler(&*eventHandler);
}

} // namespace simpleos
