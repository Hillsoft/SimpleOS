#include "layout.hpp"

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

std::vector<SegmentDefinition*> arrangeSegments(std::vector<TranslationUnit>& units) {
  std::vector<SegmentDefinition*> segments;
  uint16_t currentIndex = 0;

  for (auto& unit : units) {
    for (auto& s : unit.segments) {
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

  return segments;
}

} // namespace
