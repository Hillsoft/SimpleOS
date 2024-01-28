#pragma once

#include <cstdint>
#include <span>
#include <vector>

namespace omf {

struct RawRecord {
  uint8_t recordIdentifier;
  uint16_t recordLength;
  std::span<const uint8_t> recordContents;
  uint8_t checksum;
};

std::vector<RawRecord> extractRawRecords(std::span<const uint8_t> fileContents);

} // namespace omf
