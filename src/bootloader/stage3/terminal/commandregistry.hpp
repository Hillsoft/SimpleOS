#pragma once

#include "mysty/optional.hpp"
#include "mysty/span.hpp"

namespace simpleos {

using TerminalCommand = void (*)(mysty::Span<const mysty::StringView> args);

mysty::Optional<TerminalCommand> getCommand(mysty::StringView commandName);

} // namespace simpleos
