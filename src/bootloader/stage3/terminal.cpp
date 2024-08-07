#include "terminal.hpp"

#include "globaleventqueue.hpp"
#include "hid/keyboard.hpp"
#include "mysty/io.hpp"
#include "mysty/span.hpp"
#include "mysty/storage.hpp"
#include "mysty/string.hpp"

namespace simpleos {

namespace {

class KeyboardEventHandler : public EventHandler<hid::KeyboardEvent> {
 public:
  KeyboardEventHandler(mysty::String& currentCommand)
      : currentCommand_(currentCommand) {}

  void handleEvent(hid::KeyboardEvent event) override {
    if (event.ascii > 0 && event.type == hid::KeyboardEvent::Type::PRESS) {
      if (event.code == hid::KeyCode::ENTER) {
        mysty::putc('\n');
        mysty::puts(currentCommand_);
        mysty::putc('\n');
        currentCommand_ = "";
      } else {
        mysty::putc(event.ascii);
        currentCommand_.append(event.ascii);
      }
    }
  }

 private:
  mysty::String& currentCommand_;
};

struct TerminalState {
 public:
  explicit TerminalState() : eventHandler(currentCommand) {}

  mysty::String currentCommand;
  KeyboardEventHandler eventHandler;
};

mysty::StorageFor<TerminalState> state;

} // namespace

void startTerminal() {
  mysty::clrscr();
  constexpr mysty::StringView kWelcomeMessage = "Welcome to SimpleOS!\n\n\n";
  mysty::puts(kWelcomeMessage);

  state.emplace();
  registerHandler(&state->eventHandler);
}

} // namespace simpleos
