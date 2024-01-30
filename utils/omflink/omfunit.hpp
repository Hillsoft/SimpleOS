#pragma once

#include "omfrecord.hpp"

#include <memory>
#include <optional>
#include <string_view>

namespace omf {

struct FrameThread {
  enum class Method: uint8_t {
    SEGDEF = 0,
    GRPDEF = 1,
    EXTDEF = 2,
    PREV_SEG = 4,
    TARG_SEG = 5,
  };

  Method method;
  uint8_t index;
};

struct TargetThread {
  enum class Method: uint8_t {
    SEGDEF = 0,
    GRPDEF = 1,
    EXTDEF = 2,
    FRAME_NUMBER = 3,
    SEGDEF_ZERO_DISPLACE = 4,
    GRPDEF_ZERO_DISPLACE = 5,
    EXTDEF_ZERO_DISPLACE = 6,
  };

  Method method;
  uint8_t index;
};

struct FixupData {
  enum class RelativeTo {
    SELF_RELATIVE = 0,
    SEGMENT_RELATIVE = 1,
  };

  enum class LocationType {
    LOW_ORDER_BYTE = 0,
    OFFSET_16BIT = 1,
    SEGMENT_BASE_16BIT = 2,
    // There are other valid locations, but we don't support them
  };

  RelativeTo relativeTo;
  LocationType locationType;
  uint16_t dataRecordOffset;

  FrameThread frameThread;
  TargetThread targetThread;

  uint16_t targetDisplacement;
};

struct LogicalData {
  uint8_t segmentIndex;
  uint16_t dataOffset;
  std::span<const uint8_t> data;

  std::vector<FixupData> fixups;
};

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

  std::vector<LogicalData> dataBlocks = {};
};

struct ExportedName {
  std::string_view name;
  uint8_t segmentIndex;
  uint16_t offset;
};

struct ImportedName {
  std::string_view name;
};

struct TranslationUnit {
  std::string_view name;
  std::vector<std::string_view> namesList;
  std::vector<SegmentDefinition> segments;
  std::vector<ExportedName> exports;
  std::vector<ImportedName> imports;

  std::unique_ptr<const uint8_t[]> rawBytes;
};

TranslationUnit decodeUnit(std::span<const uint8_t> fileContents, std::unique_ptr<const uint8_t[]> rawBytes);
TranslationUnit decodeUnit(const std::vector<RawRecord>& records, std::unique_ptr<const uint8_t[]> rawBytes);

} // namespace omf
