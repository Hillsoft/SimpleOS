#pragma once

#include "mysty/int.hpp"
#include "mysty/optional.hpp"
#include "mysty/span.hpp"

namespace simpleos {

class File {
 public:
  ~File();

  void close();

 private:
  explicit File(uint8_t handle);

  friend mysty::Optional<File> openFile(mysty::StringView path);

  uint8_t handle_;
};

bool initializeFileSystem();

mysty::Optional<File> openFile(mysty::StringView path);

} // namespace simpleos
