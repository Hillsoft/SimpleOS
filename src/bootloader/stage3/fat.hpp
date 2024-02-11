#pragma once

#include "mysty/int.hpp"
#include "mysty/optional.hpp"
#include "mysty/span.hpp"

namespace simpleos {

class File {
 public:
  enum class ReadResult {
    OK,
    REACHED_END,
    FAILED,
  };

  ~File();

  size_t remainingBytes() const;
  size_t position() const;
  size_t size() const;

  ReadResult read(mysty::Span<uint8_t> outBuffer);
  void close();

 private:
  explicit File(uint8_t handle);

  friend mysty::Optional<File> openFile(mysty::StringView path);

  uint8_t handle_;
};

bool initializeFileSystem();

mysty::Optional<File> openFile(mysty::StringView path);

} // namespace simpleos
