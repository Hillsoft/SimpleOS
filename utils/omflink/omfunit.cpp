#include "omfunit.hpp"

#include <cassert>
#include <format>
#include <stdexcept>

namespace omf {

namespace {

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

struct NamesRecord {
  std::vector<std::string_view> names;
};

NamesRecord parseNamesRecord(const RawRecord& record) {
  assert(record.recordIdentifier == 0x96);

  std::vector<std::string_view> names;

  std::span<const uint8_t> contents = record.recordContents;
  while (contents.size() > 0) {
    uint8_t nameLength = contents[0];

    if (contents.size() < 1 + nameLength) {
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

} // namespace

TranslationUnit decodeUnit(std::span<const uint8_t> fileContents) {
  return decodeUnit(extractRawRecords(fileContents));
}

TranslationUnit decodeUnit(const std::vector<RawRecord>& records) {
  if (records.size() == 0) {
    throw std::runtime_error{"No records"};
  }

  TranslationUnit result;

  // The first records must be a header
  if (records[0].recordIdentifier != 0x80) {
    throw std::runtime_error{"First records must be THEADR"};
  }
  RecordHeader header = parseRecordHeader(records[0]);
  result.name = header.unitName;

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
