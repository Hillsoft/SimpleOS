#include "terminal.hpp"

#include "globaleventqueue.hpp"
#include "hid/keyboard.hpp"
#include "mysty/io.hpp"
#include "mysty/span.hpp"
#include "mysty/storage.hpp"
#include "mysty/string.hpp"
#include "mysty/stringutils.hpp"
#include "mysty/vector.hpp"
#include "terminal/commandregistry.hpp"

namespace simpleos {

namespace {

void executeCommand(mysty::String fullCommand) {
  mysty::Vector<mysty::StringView> commandParts =
      mysty::splitStringView(fullCommand, ' ');
  if (commandParts.size() == 0) {
    // no command to execute
    return;
  }

  mysty::Optional<TerminalCommand> command = getCommand(commandParts[0]);

  if (!command.has_value()) {
    mysty::printf("Command '%s' not found", commandParts[0].get(0));
    return;
  }

  (*command)(commandParts);
}

class KeyboardEventHandler : public EventHandler<hid::KeyboardEvent> {
 public:
  KeyboardEventHandler(mysty::String& currentCommand)
      : currentCommand_(currentCommand) {}

  void handleEvent(hid::KeyboardEvent event) override {
    if (event.ascii > 0 && event.type == hid::KeyboardEvent::Type::PRESS) {
      if (event.code == hid::KeyCode::ENTER) {
        mysty::putc('\n');
        mysty::String finalCommand = "";
        finalCommand.exchange(currentCommand_);
        executeCommand(mysty::move(finalCommand));
        constexpr mysty::StringView nextCommandPrompt = "\n> ";
        mysty::puts(nextCommandPrompt);
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
  constexpr mysty::StringView kWelcomeMessage = "Welcome to SimpleOS!\n\n\n> ";
  mysty::puts(kWelcomeMessage);

  state.emplace();
  registerHandler(&state->eventHandler);
}

} // namespace simpleos
