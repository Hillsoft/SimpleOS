#include "terminal/commandregistry.hpp"

#include "fat.hpp"
#include "mysty/array.hpp"
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

void catCommand(mysty::Span<const mysty::StringView> args) {
  if (args.size() < 2) {
    constexpr mysty::StringView kTooFewArgs = "Usage: cat <file_name>";
    mysty::puts(kTooFewArgs);
  }

  mysty::StringView fileName = args[1];

  mysty::Optional<File> file = openFile(fileName);
  if (!file.has_value()) {
    constexpr mysty::StringView errorMessage{"Failed to open 'test.txt'\n"};
    mysty::puts(errorMessage);
    return;
  }

  mysty::FixedArray<uint8_t, 100> readBuffer;
  File::ReadResult result = File::ReadResult::OK;

  while (result == File::ReadResult::OK) {
    size_t remaining = file->remainingBytes();

    result = file->read(readBuffer);

    if (result == File::ReadResult::FAILED) {
      constexpr mysty::StringView errorMessage{"\nFailed to read file\n"};
      mysty::puts(errorMessage);
      return;
    }

    mysty::puts(static_cast<mysty::Span<uint8_t>>(readBuffer)
                    .reinterpret<const char>()
                    .slice_front(0, remaining));
  }
}

} // namespace

mysty::Optional<TerminalCommand> getCommand(mysty::StringView commandName) {
  if (commandName == "echo") {
    return echoCommand;
  }
  if (commandName == "cat") {
    return catCommand;
  }

  return mysty::Optional<TerminalCommand>{};
}

} // namespace simpleos
