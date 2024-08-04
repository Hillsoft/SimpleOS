#include "terminal.hpp"

#include "mysty/io.hpp"
#include "mysty/span.hpp"

namespace simpleos {

void startTerminal() {
  mysty::clrscr();
  constexpr mysty::StringView kWelcomeMessage = "Welcome to SimpleOS!\n\n\n";
  mysty::puts(kWelcomeMessage);
}

} // namespace simpleos
