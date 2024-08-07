#include "terminal/commandregistry.hpp"

namespace simpleos {

mysty::Optional<TerminalCommand> getCommand(mysty::StringView commandName) {
  return mysty::Optional<TerminalCommand>{};
}

} // namespace simpleos
