#include <iostream>
#include <fstream>
#include <vector>

#include "omfrecord.hpp"
#include "omfunit.hpp"

using namespace omf;

int main(int argc, char** argv) {
  if (argc < 2) {
    std::cout << "Syntax: " << argv[0] << " <obj file>" << std::endl;
    return -1;
  }

  std::ifstream file{argv[1], std::ios::in | std::ios::binary | std::ios::ate};
  if (!file.is_open()) {
    std::cerr << "Cannot open object file " << argv[1] << std::endl;
    return -1;
  }

  std::streamsize fileSize = file.tellg();
  file.seekg(0, std::ios::beg);

  const std::vector<uint8_t> buffer = [&]() {
    std::vector<uint8_t> buffer;
    buffer.resize(fileSize);
    if (!file.read(reinterpret_cast<char*>(buffer.data()), fileSize)) {
      throw std::runtime_error{"Cannot read file"};
    }
    return buffer;
  }();

  std::cout << "Read " << fileSize << " bytes" << std::endl;

  const std::vector<RawRecord> rawRecords = extractRawRecords(buffer);

  std::cout << "Extracted " << rawRecords.size() << " records" << std::endl;

  for (const auto& record : rawRecords) {
    std::cout << "Record identifier: " << std::hex << (int)record.recordIdentifier << std::dec << std::endl;
  }

  TranslationUnit unit = decodeUnit(rawRecords);

  std::cout << "Unit provides symbols:" << std::endl;
  for (const auto& symbol : unit.exports) {
    std::cout << "  " << symbol.name << std::endl;
  }

  return 0;
}
