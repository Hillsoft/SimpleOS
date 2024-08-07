#include "terminal/commandregistry.hpp"

#include "mysty/io.hpp"

namespace simpleos {

namespace {

void echoCommand(mysty::Span<const mysty::StringView> args) {
  args = args.slice_front(1);
  for (const auto& a : args) {
    mysty::puts(a);
    mysty::putc(' ');
  }
}

} // namespace

mysty::Optional<TerminalCommand> getCommand(mysty::StringView commandName) {
  if (commandName == "echo") {
    return echoCommand;
  }

  return mysty::Optional<TerminalCommand>{};
}

} // namespace simpleos
