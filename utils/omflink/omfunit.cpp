#include "omfunit.hpp"

#include <cassert>
#include <format>
#include <stdexcept>

namespace omf {

namespace {

std::string_view lookupName(const TranslationUnit& unit, uint8_t nameIndex) {
  if (nameIndex == 0 || nameIndex > unit.namesList.size()) {
    throw std::runtime_error{"Out of bounds name index"};
  }

  return unit.namesList[nameIndex - 1];
}

SegmentDefinition& lookupSegment(TranslationUnit& unit, uint8_t segmentIndex) {
  if (segmentIndex == 0 || segmentIndex > unit.segments.size()) {
    throw std::runtime_error{"Out of bounds segment index"};
  }

  return unit.segments[segmentIndex - 1];
}

const SegmentDefinition& lookupSegment(const TranslationUnit& unit, uint8_t segmentIndex) {
  if (segmentIndex == 0 || segmentIndex > unit.segments.size()) {
    throw std::runtime_error{"Out of bounds segment index"};
  }

  return unit.segments[segmentIndex - 1];
}

struct RecordHeader {
  std::string_view unitName;
};

RecordHeader parseRecordHeader(const RawRecord& record) {
  assert(record.recordIdentifier == 0x80);

  uint8_t strLength = record.recordContents[0];
  if (strLength != record.recordLength - 2) {
    throw std::runtime_error{"Incorrectly formatted THEADR record"};
  }

  return RecordHeader{
    .unitName={
      reinterpret_cast<const char*>(&*(record.recordContents.begin() + 1)),
      reinterpret_cast<const char*>(&*record.recordContents.end())}
  };
}

struct CommentRecord {};

CommentRecord parseCommentRecord(const RawRecord& record) {
  assert(record.recordIdentifier == 0x88);
  return {};
}

struct ImportRecord {
  std::vector<ImportedName> importedNames;
};

ImportRecord parseImportRecord(const RawRecord& record) {
  assert(record.recordIdentifier == 0x8c);

  std::vector<ImportedName> names;

  std::span<const uint8_t> contents = record.recordContents;
  while (contents.size() > 0) {
    uint8_t nameLength = contents[0];
    if (contents.size() < nameLength + 2u) {
      throw std::runtime_error{"Incomplete import"};
    }

    std::string_view name{
      reinterpret_cast<const char*>(contents.data() + 1),
      nameLength
    };

    names.emplace_back(name);

    contents = contents.subspan(nameLength + 2);
  }

  return ImportRecord{std::move(names)};
}

struct ExportRecord {
  std::vector<ExportedName> exportedNames;
};

ExportRecord parseExportRecord(const RawRecord& record) {
  assert(record.recordIdentifier == 0x90);

  std::vector<ExportedName> names;

  if (record.recordLength < 3) {
    throw std::runtime_error{"Incorrectly formatted PUBDEF"};
  }

  uint8_t groupIndex = record.recordContents[0];
  uint8_t segmentIndex = record.recordContents[1];

  std::span<const uint8_t> contents = record.recordContents.subspan(segmentIndex == 0 ? 4 : 2);
  while (contents.size() > 0) {
    uint8_t nameLength = contents[0];
    if (contents.size() < nameLength + 4u) {
      throw std::runtime_error{"Incomplete export"};
    }

    std::string_view name{
      reinterpret_cast<const char*>(contents.data() + 1),
      nameLength
    };

    uint16_t offset = *reinterpret_cast<const uint16_t*>(contents.data() + 1 + nameLength);

    names.push_back(ExportedName{
      .name = name,
      .segmentIndex = segmentIndex,
      .offset = offset,
    });

    contents = contents.subspan(nameLength + 4);
  }

  return ExportRecord{std::move(names)};
}

struct NamesRecord {
  std::vector<std::string_view> names;
};

NamesRecord parseNamesRecord(const RawRecord& record) {
  assert(record.recordIdentifier == 0x96);

  std::vector<std::string_view> names;

  std::span<const uint8_t> contents = record.recordContents;
  while (contents.size() > 0) {
    uint8_t nameLength = contents[0];

    if (contents.size() < 1u + nameLength) {
      throw std::runtime_error{"Incomplete name"};
    }

    names.emplace_back(
      reinterpret_cast<const char*>(&*(record.recordContents.begin() + 1)),
      nameLength);

    contents = contents.subspan(1 + nameLength);
  }

  return NamesRecord{
    .names = std::move(names)
  };
}

struct SegmentRecord {
  SegmentDefinition::Alignment alignment;
  SegmentDefinition::Combination combination;
  SegmentDefinition::BitField bitField;

  uint32_t segmentLength;
  uint8_t segmentNameIndex;
  uint8_t classNameIndex;
  uint8_t overlayNameIndex;
};

SegmentRecord parseSegmentRecord(const RawRecord& record) {
  assert(record.recordIdentifier == 0x98);

  if (record.recordLength != 7) {
    throw std::runtime_error{"Incorrectly formatted SEGDEF"};
  }

  uint8_t attributes = record.recordContents[0];
  uint32_t segmentLength = *reinterpret_cast<const uint16_t*>(record.recordContents.data() + 1);
  uint8_t segmentNameIndex = record.recordContents[3];
  uint8_t classNameIndex = record.recordContents[4];
  uint8_t overlayNameIndex = record.recordContents[5];

  // The high 3 bits
  SegmentDefinition::Alignment alignment = static_cast<SegmentDefinition::Alignment>(attributes >> 5);
  if (alignment != SegmentDefinition::Alignment::ABSOLUTE
      && alignment != SegmentDefinition::Alignment::RELOCATABLE_BYTE_ALIGNED
      && alignment != SegmentDefinition::Alignment::RELOCATABLE_2BYTE_ALIGNED
      && alignment != SegmentDefinition::Alignment::RELOCATABLE_16BYTE_ALIGNED) {
    throw std::runtime_error{"Invalid segment alignment"};
  }

  SegmentDefinition::Combination combination = static_cast<SegmentDefinition::Combination>((attributes >> 2) & 0b111);
  if (combination != SegmentDefinition::Combination::PRIVATE
      && combination != SegmentDefinition::Combination::PUBLIC
      && combination != SegmentDefinition::Combination::STACK
      && combination != SegmentDefinition::Combination::COMMON) {
    throw std::runtime_error{"Invalid segment combination"};
  }

  bool big = (attributes & 0b10) > 0;

  SegmentDefinition::BitField bitField = (attributes & 0b1) > 0
      ? SegmentDefinition::BitField::BITS_32
      : SegmentDefinition::BitField::BITS_16;

  if (big) {
    if (segmentLength != 0) {
      throw std::runtime_error{"Bad segment length"};
    }
    segmentLength = 64 * 1024; // 64kb
  }

  return SegmentRecord{
    .alignment = alignment,
    .combination = combination,
    .bitField = bitField,
    .segmentLength = segmentLength,
    .segmentNameIndex = segmentNameIndex,
    .classNameIndex = classNameIndex,
    .overlayNameIndex = overlayNameIndex,
  };
}

bool isValidFrameThreadMethod(FrameThread::Method method) {
  return method == FrameThread::Method::SEGDEF
      || method == FrameThread::Method::GRPDEF
      || method == FrameThread::Method::EXTDEF
      || method == FrameThread::Method::PREV_SEG
      || method == FrameThread::Method::TARG_SEG;
}

bool isValidTargetThreadMethod(TargetThread::Method method) {
  return method == TargetThread::Method::SEGDEF
      || method == TargetThread::Method::GRPDEF
      || method == TargetThread::Method::EXTDEF
      || method == TargetThread::Method::FRAME_NUMBER
      || method == TargetThread::Method::SEGDEF_ZERO_DISPLACE
      || method == TargetThread::Method::GRPDEF_ZERO_DISPLACE
      || method == TargetThread::Method::EXTDEF_ZERO_DISPLACE;
}

struct FixupRecord {
  std::vector<FixupData> fixups;
};

FixupRecord parseFixupRecord(const RawRecord& record) {
  std::vector<FixupData> result;

  std::array<std::variant<bool, FrameThread, TargetThread>, 4> threads;

  std::span<const uint8_t> contents = record.recordContents;
  while (contents.size() > 0) {
    if ((contents[0] & 0b10000000) == 0) {
      // Current record is a thread
      uint8_t headerByte = contents[0];
      contents = contents.subspan(1);

      bool isFrameThread = (0b1000000 & headerByte) > 0;

      uint8_t tIndex = headerByte & 0b11;

      if (isFrameThread) {
        FrameThread::Method method = static_cast<FrameThread::Method>((headerByte >> 2) & 0b111);

        if (!isValidFrameThreadMethod(method)) {
          throw std::runtime_error{"Invalid frame thread method"};
        }

        uint8_t index = 0;
        if (method == FrameThread::Method::SEGDEF
            || method == FrameThread::Method::GRPDEF
            || method == FrameThread::Method::EXTDEF) {
          // we need to load index
          if (contents.size() < 1) {
            throw std::runtime_error{"Invalid thread format"};
          }
          index = contents[0];
          contents = contents.subspan(1);
        }

        threads[tIndex] = FrameThread{
          .method = method,
          .index = index,
        };
      }
      else {
        TargetThread::Method method = static_cast<TargetThread::Method>((headerByte >> 2) & 0b111);

        if (!isValidTargetThreadMethod(method)) {
          throw std::runtime_error{"Invalid target thread method"};
        }

        uint8_t index = 0;
        if (method == TargetThread::Method::SEGDEF
            || method == TargetThread::Method::GRPDEF
            || method == TargetThread::Method::EXTDEF
            || method == TargetThread::Method::SEGDEF_ZERO_DISPLACE
            || method == TargetThread::Method::GRPDEF_ZERO_DISPLACE
            || method == TargetThread::Method::EXTDEF_ZERO_DISPLACE) {
          // we need to load index
          if (contents.size() < 1) {
            throw std::runtime_error{"Invalid thread format"};
          }
          index = contents[0];
          contents = contents.subspan(1);
        }

        threads[tIndex] = TargetThread{
          .method = method,
          .index = index,
        };
      }
    }
    else {
      // Current record is a fixup
      if (contents.size() < 3) {
        throw std::runtime_error{"Invalid fixup"};
      }

      uint8_t headerHi = contents[0];
      uint8_t headerLo = contents[1];

      FixupData::RelativeTo relativeTo = static_cast<FixupData::RelativeTo>(0b1 & (headerHi >> 6));
      if (relativeTo != FixupData::RelativeTo::SELF_RELATIVE
          && relativeTo != FixupData::RelativeTo::SEGMENT_RELATIVE) {
        throw std::runtime_error{"Invalid fixup relativeness"};
      }

      FixupData::LocationType location = static_cast<FixupData::LocationType>(0b1111 & (headerHi >> 2));
      if (location != FixupData::LocationType::LOW_ORDER_BYTE
          && location != FixupData::LocationType::OFFSET_16BIT
          && location != FixupData::LocationType::SEGMENT_BASE_16BIT) {
        throw std::runtime_error{"Invalid fixup location type"};
      }

      uint16_t dataRecordOffset = (static_cast<uint16_t>(headerHi & 0b11) << 8) | static_cast<uint16_t>(headerLo);


      uint8_t fixDataByte = contents[2];
      bool useFrameThread = (fixDataByte & 0b10000000) > 0;
      uint8_t frameBits = 0b111 & (fixDataByte >> 4);
      bool useTargetThread = (fixDataByte & 0b1000) > 0;
      uint8_t targetBits = 0b111 & fixDataByte;
      bool targetDisplacementAbsent = (fixDataByte & 0b100) > 0;

      contents = contents.subspan(3);

      FrameThread frame;
      if (useFrameThread) {
        uint8_t threadIndex = frameBits & 0b11;
        if (!std::holds_alternative<FrameThread>(threads[threadIndex])) {
          throw std::runtime_error{"Fixup referenced bad thread"};
        }

        frame = std::get<FrameThread>(threads[threadIndex]);
      }
      else {
        FrameThread::Method method = static_cast<FrameThread::Method>(frameBits);
        if (!isValidFrameThreadMethod(method)) {
          throw std::runtime_error{"Invalid fixup method"};
        }

        uint8_t index = 0;
        if (method == FrameThread::Method::SEGDEF
            || method == FrameThread::Method::GRPDEF
            || method == FrameThread::Method::EXTDEF) {
          if (contents.size() < 1) {
            throw std::runtime_error{"Invalid fixup"};
          }
          index = contents[0];
          contents = contents.subspan(1);
        }

        frame = FrameThread{
          .method = method,
          .index = index,
        };
      }

      TargetThread target;
      if (useTargetThread) {
        uint8_t threadIndex = targetBits & 0b11;
        if (!std::holds_alternative<TargetThread>(threads[threadIndex])) {
          throw std::runtime_error{"Fixup referenced bad thread"};
        }

        target = std::get<TargetThread>(threads[threadIndex]);
      }
      else {
        TargetThread::Method method = static_cast<TargetThread::Method>(targetBits & 0b111);
        if (!isValidTargetThreadMethod(method)) {
          std::string error = std::format("Invalid fixup method: {}", static_cast<uint16_t>(method));
          throw std::runtime_error{error};
        }

        if (contents.size() < 1) {
          throw std::runtime_error{"Invalid fixup"};
        }
        uint8_t index = contents[0];
        contents = contents.subspan(1);

        target = TargetThread{
          .method = method,
          .index = index,
        };
      }

      uint16_t targetDisplacement = 0;
      if (!targetDisplacementAbsent) {
        if (contents.size() < 2) {
          throw std::runtime_error{"Invalid fixup"};
        }
        targetDisplacement = *reinterpret_cast<const uint16_t*>(contents.data());
        contents = contents.subspan(2);
      }

      result.push_back(FixupData{
        .relativeTo = relativeTo,
        .locationType = location,
        .dataRecordOffset = dataRecordOffset,
        .frameThread = frame,
        .targetThread = target,
        .targetDisplacement = targetDisplacement,
      });
    }
  }

  return FixupRecord{std::move(result)};
}

struct EnumeratedDataRecord {
  LogicalData data;
};

EnumeratedDataRecord parseEnumeratedDataRecord(const RawRecord& record) {
  assert(record.recordIdentifier == 0xa0);

  if (record.recordLength < 4) {
    throw std::runtime_error{"Incorrectly formatted LEDATA"};
  }

  return EnumeratedDataRecord{
    .data = LogicalData{
      .segmentIndex = record.recordContents[0],
      .dataOffset = *reinterpret_cast<const uint16_t*>(record.recordContents.data() + 1),
      .data = record.recordContents.subspan(3)
    }
  };
}

} // namespace

TranslationUnit decodeUnit(std::span<const uint8_t> fileContents, std::unique_ptr<const uint8_t[]> rawBytes) {
  return decodeUnit(extractRawRecords(fileContents), std::move(rawBytes));
}

TranslationUnit decodeUnit(const std::vector<RawRecord>& records, std::unique_ptr<const uint8_t[]> rawBytes) {
  if (records.size() == 0) {
    throw std::runtime_error{"No records"};
  }

  TranslationUnit result{
    .name = "",
    .namesList = {},
    .segments = {},
    .exports = {},
    .imports = {},
    .rawBytes = std::move(rawBytes),
  };

  // The first records must be a header
  if (records[0].recordIdentifier != 0x80) {
    throw std::runtime_error{"First records must be THEADR"};
  }
  RecordHeader header = parseRecordHeader(records[0]);
  result.name = header.unitName;

  LogicalData* lastDataBlock = nullptr;

  for (auto it = records.begin() + 1; it != records.end(); it++) {
    const auto& currentRecord = *it;

    switch (currentRecord.recordIdentifier) {
     case 0x80:
      {
        // THEADR
        throw std::runtime_error{"Multiple THEADR not allowed"};
      }

     case 0x88:
      {
        // COMENT
        CommentRecord record = parseCommentRecord(currentRecord);
        // nothing to do with a comment
        break;
      }

     case 0x8a:
      {
        // MODEND
        break;
      }

     case 0x8c:
      {
        // EXTDEF
        ImportRecord record = parseImportRecord(currentRecord);
        result.imports.reserve(result.imports.size() + record.importedNames.size());
        for (const auto& n : record.importedNames) {
          result.imports.emplace_back(std::move(n));
        }
        break;
      }

     case 0x90:
      {
        // PUBDEF
        ExportRecord record = parseExportRecord(currentRecord);
        result.exports.reserve(result.exports.size() + record.exportedNames.size());
        for (const auto& n : record.exportedNames) {
          // validate segment
          if (n.segmentIndex == 0 || n.segmentIndex > result.segments.size()) {
            throw std::runtime_error{"Invalid segment index in exported name"};
          }
          result.exports.emplace_back(std::move(n));
        }
        break;
      }

     case 0x96:
      {
        // LNAMES
        NamesRecord record = parseNamesRecord(currentRecord);
        result.namesList.reserve(result.namesList.size() + record.names.size());
        for (const auto& n : record.names) {
          result.namesList.emplace_back(std::move(n));
        }
        break;
      }

     case 0x98:
      {
        // SEGDEF
        SegmentRecord record = parseSegmentRecord(currentRecord);
        result.segments.push_back(SegmentDefinition{
          .alignment = record.alignment,
          .combination = record.combination,
          .bitField = record.bitField,
          .segmentLength = record.segmentLength,
          .segmentName = lookupName(result, record.segmentNameIndex),
          .className = lookupName(result, record.classNameIndex),
        });
        break;
      }

     case 0x9c:
      {
        // FIXUPP
        FixupRecord record = parseFixupRecord(currentRecord);
        break;
      }

     case 0xa0:
      {
        // LEDATA
        EnumeratedDataRecord record = parseEnumeratedDataRecord(currentRecord);
        SegmentDefinition& segment = lookupSegment(result, record.data.segmentIndex);
        lastDataBlock = &segment.dataBlocks.emplace_back(std::move(record.data));
        break;
      }

     default:
      {
        std::string error = std::format("Unrecognised record identifier: {:x}", currentRecord.recordIdentifier);
        throw std::runtime_error{error};
      }
    }
  }

  return result;
}

} // namespace omf
