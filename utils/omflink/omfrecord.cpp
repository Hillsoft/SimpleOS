#include "omfrecord.hpp"

#include <stdexcept>

namespace omf {

namespace {

bool verifyChecksum(const RawRecord& record) {
  if (record.checksum == 0) {
    return true;
  }

  uint8_t sum = 0;
  sum += record.recordIdentifier;
  sum += (record.recordLength & 0xff) + (record.recordLength >> 8);
  for (auto& byte : record.recordContents) {
    sum += byte;
  }
  sum += record.checksum;

  return sum == 0;
}

} // namespace

std::vector<RawRecord> extractRawRecords(std::span<const uint8_t> fileContents) {
  std::vector<RawRecord> result;

  while (fileContents.size() > 0) {
    if (fileContents.size() < 4) {
      throw std::runtime_error{"Incomplete record"};
    }

    RawRecord currentRecord;
    currentRecord.recordIdentifier = fileContents[0];
    currentRecord.recordLength = *reinterpret_cast<const uint16_t*>(fileContents.data() + 1);

    if (currentRecord.recordLength == 0) {
      throw std::runtime_error{"Undersized record"};
    }
    if (currentRecord.recordLength > 1021) {
      throw std::runtime_error{"Oversized record"};
    }
    if (fileContents.size() < currentRecord.recordLength + 1u) {
      throw std::runtime_error{"Incomplete record"};
    }

    currentRecord.recordContents = std::span<const uint8_t>{fileContents.begin() + 3, static_cast<uint16_t>(currentRecord.recordLength - 1)};
    currentRecord.checksum = fileContents[currentRecord.recordLength + 2];

    if (!verifyChecksum(currentRecord)) {
      throw std::runtime_error{"Bad checksum"};
    }

    result.push_back(std::move(currentRecord));

    fileContents = fileContents.subspan(currentRecord.recordLength + 3);
  }

  return result;
}

} // namespace omf
