#include <cstring>
#include <fstream>
#include <iostream>
#include <vector>

#include "fixup.hpp"
#include "layout.hpp"
#include "nameset.hpp"
#include "objloader.hpp"

using namespace omf;

int main(int argc, char** argv) {
  if (argc < 3) {
    std::cout << "Syntax: " << argv[0] << " <out file> <obj file>" << std::endl;
    return -1;
  }

  std::vector<TranslationUnit> translationUnits;
  translationUnits.reserve(static_cast<size_t>(argc - 1));

  NameSet nameSet;

  for (int i = 2; i < argc; i++) {
    const TranslationUnit& unit = translationUnits.emplace_back(loadUnitFromFilename(argv[i]));

    std::cout << unit.name << std::endl;

    std::cout << "  Unit provides symbols:" << std::endl;
    for (const auto& symbol : unit.exports) {
      std::cout << "    " << symbol.name << std::endl;
    }
    std::cout << std::endl;

    std::cout << "  Unit requires symbols:" << std::endl;
    for (const auto& symbol : unit.imports) {
      std::cout << "    " << symbol.name << std::endl;
    }
    std::cout << std::endl;

    nameSet.registerTranslationUnit(unit);
  }

  std::vector<SegmentDefinition*> segmentLayout = arrangeSegments(translationUnits);
  for (const auto& s : segmentLayout) {
    std::cout << "Segment " << s->segmentName << " at 0x" << std::hex << s->baseAddress << std::dec << std::endl;
  }

  if (segmentLayout.size() == 0) {
    throw std::runtime_error{"No segments to output"};
  }

  std::size_t outSize = segmentLayout.back()->baseAddress;
  for (const auto& d : segmentLayout.back()->dataBlocks) {
    outSize += d.data.size();
  }
  std::unique_ptr<uint8_t[]> outBuffer = std::make_unique_for_overwrite<uint8_t[]>(outSize);
  // 0 this as we will not fill padding later
  std::memset(outBuffer.get(), 0, outSize);

  for (const auto& u : translationUnits) {
    std::cout << "Writing " << u.name << std::endl;
    writeTranslationUnit(outBuffer.get(), nameSet, u);
  }

  std::ofstream outFile{argv[1], std::ios::out | std::ios::binary};
  outFile.write(reinterpret_cast<char*>(outBuffer.get()), static_cast<std::streamsize>(outSize));

  return 0;
}
