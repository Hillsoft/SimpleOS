#include <cstring>
#include <fstream>
#include <iostream>
#include <vector>

#include "fixup.hpp"
#include "layout.hpp"
#include "linkscript.hpp"
#include "nameset.hpp"
#include "objloader.hpp"

using namespace omf;

int main(int argc, char** argv) {
  if (argc < 4) {
    std::cout << "Syntax: " << argv[0] << " <link script> <out file> <obj file>"
              << std::endl;
    return -1;
  }

  LinkScript linkScript = loadLinkScript(argv[1]);

  std::vector<TranslationUnit> translationUnits;
  translationUnits.reserve(static_cast<size_t>(argc - 1));

  NameSet nameSet;

  for (int i = 3; i < argc; i++) {
    const TranslationUnit& unit =
        translationUnits.emplace_back(loadUnitFromFilename(argv[i]));

    nameSet.registerTranslationUnit(unit);
  }

  std::vector<SegmentDefinition*> segmentLayout =
      arrangeSegments(linkScript, translationUnits);

  if (segmentLayout.size() == 0) {
    throw std::runtime_error{"No segments to output"};
  }

  std::size_t outSize = segmentLayout.back()->baseAddress;
  for (const auto& d : segmentLayout.back()->dataBlocks) {
    outSize += d.data.size();
  }
  std::unique_ptr<uint8_t[]> outBuffer =
      std::make_unique_for_overwrite<uint8_t[]>(outSize);
  // 0 this as we will not fill padding later
  std::memset(outBuffer.get(), 0, outSize);

  for (const auto& u : translationUnits) {
    writeTranslationUnit(outBuffer.get(), nameSet, u);
  }

  std::ofstream outFile{argv[2], std::ios::out | std::ios::binary};
  outFile.write(
      reinterpret_cast<char*>(outBuffer.get()),
      static_cast<std::streamsize>(outSize));

  return 0;
}
