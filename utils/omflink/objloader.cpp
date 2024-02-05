#include "objloader.hpp"

// #include <format>
#include <fstream>
#include <memory>
#include <stdexcept>

namespace omf {

TranslationUnit loadUnitFromFilename(const char* filename) {
  std::ifstream file{filename, std::ios::in | std::ios::binary | std::ios::ate};
  if (!file.is_open()) {
    // throw std::runtime_error{std::format("Cannot open object file {}",
    // filename)};
  }

  std::streamsize fileSize = file.tellg();
  file.seekg(0, std::ios::beg);

  std::unique_ptr<uint8_t[]> buffer =
      std::make_unique_for_overwrite<uint8_t[]>(static_cast<size_t>(fileSize));
  if (!file.read(reinterpret_cast<char*>(&buffer[0]), fileSize)) {
    throw std::runtime_error{"Cannot read file"};
  }

  std::span<const uint8_t> bufferRef{
      &buffer[0], static_cast<std::size_t>(fileSize)};
  return decodeUnit(bufferRef, std::move(buffer));
}

} // namespace omf
