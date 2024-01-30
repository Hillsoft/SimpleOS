#include <iostream>
#include <vector>

#include "layout.hpp"
#include "nameset.hpp"
#include "objloader.hpp"

using namespace omf;

int main(int argc, char** argv) {
  if (argc < 2) {
    std::cout << "Syntax: " << argv[0] << " <obj file>" << std::endl;
    return -1;
  }

  std::vector<TranslationUnit> translationUnits;
  translationUnits.reserve(static_cast<size_t>(argc - 1));

  NameSet nameSet;

  for (int i = 1; i < argc; i++) {
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

  return -1;
}
