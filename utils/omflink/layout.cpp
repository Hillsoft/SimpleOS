#include "layout.hpp"

#include <format>

namespace omf {

namespace {

uint16_t getPaddingBytes(uint16_t currentIndex, SegmentDefinition::Alignment alignment) {
  switch (alignment) {
   case SegmentDefinition::Alignment::ABSOLUTE:
    throw std::runtime_error{"Absolute positioned segments not supported"};
    break;

   case SegmentDefinition::Alignment::RELOCATABLE_BYTE_ALIGNED:
    return 0;

   case SegmentDefinition::Alignment::RELOCATABLE_2BYTE_ALIGNED:
    return (2 - currentIndex) % 2;

   case SegmentDefinition::Alignment::RELOCATABLE_16BYTE_ALIGNED:
    return (16 - currentIndex) % 16;

   default:
    throw std::runtime_error{"Unsupported segment alignment"};
  }
}

} // namespace

std::vector<SegmentDefinition*> arrangeSegments(const LinkScript& linkScript, std::vector<TranslationUnit>& units) {
  std::vector<SegmentDefinition*> segments;
  uint16_t currentIndex = 0;

  for (const auto& scriptItem : linkScript) {
    if (std::holds_alternative<LinkScriptAddress>(scriptItem)) {
      uint16_t scriptAddr = std::get<LinkScriptAddress>(scriptItem).address;

      if (scriptAddr > currentIndex) {
        throw std::runtime_error{std::format("Fixed address {:x} from link script leaves insufficient space for earlier sections", scriptAddr)};
      }

      currentIndex = scriptAddr;
    }
    else if (std::holds_alternative<LinkScriptSection>(scriptItem)) {
      std::string_view currentSectionName = std::get<LinkScriptSection>(scriptItem).name;
      for (auto& unit : units) {
        for (auto& s : unit.segments) {
          if (s.segmentName == currentSectionName) {
            segments.push_back(&s);

            currentIndex += getPaddingBytes(currentIndex, s.alignment);

            s.baseAddress = currentIndex;

            uint16_t segmentLength = 0;
            for (const auto& d : s.dataBlocks) {
              segmentLength += d.data.size();
            }
            currentIndex += segmentLength;
          }
        }
      }
    }
    else {
      throw std::runtime_error{"Unsupported link script item"};
    }
  }

  // Validate all segments have addresses
  for (const auto& unit : units) {
    for (const auto& s : unit.segments) {
      if (s.baseAddress == 0xffff) {
        throw std::runtime_error{std::format("Section {} defined in {} is missing from link script", s.segmentName, unit.name)};
      }
    }
  }

  return segments;
}

} // namespace
