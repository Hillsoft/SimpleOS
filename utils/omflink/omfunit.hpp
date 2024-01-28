#pragma once

#include "omfrecord.hpp"

#include <string_view>

namespace omf {

struct SegmentDefinition {
  enum class Alignment: uint8_t {
    ABSOLUTE = 0,
    RELOCATABLE_BYTE_ALIGNED = 1,
    RELOCATABLE_2BYTE_ALIGNED = 2,
    RELOCATABLE_16BYTE_ALIGNED = 3,
  };

  enum class Combination: uint8_t {
    PRIVATE = 0,
    PUBLIC = 2,
    STACK = 5,
    COMMON=6,
  };

  enum class BitField: uint8_t {
    BITS_16 = 0,
    BITS_32 = 1,
  };

  Alignment alignment;
  Combination combination;
  BitField bitField;

  uint32_t segmentLength;
  std::string_view segmentName;
  std::string_view className;
};

struct TranslationUnit {
  std::string_view name;
  std::vector<std::string_view> namesList;
  std::vector<SegmentDefinition> segments;
};

TranslationUnit decodeUnit(std::span<const uint8_t> fileContents);
TranslationUnit decodeUnit(const std::vector<RawRecord>& records);

} // namespace omf
