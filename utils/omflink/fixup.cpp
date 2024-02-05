#include "fixup.hpp"

#include <cstring>
// #include <format>

namespace omf {

namespace {

uint16_t getTargetAddress(
    const FixupData& fixup,
    const NameSet& globalNames,
    uint16_t currentSegmentBase,
    const TranslationUnit& unit) {
  switch (fixup.targetThread.method) {
    case TargetThread::Method::SEGDEF:
    case TargetThread::Method::SEGDEF_ZERO_DISPLACE: {
      const auto& targetSegment = unit.segments[fixup.targetThread.index - 1];
      return targetSegment.baseAddress + fixup.targetDisplacement;
    }

    case TargetThread::Method::EXTDEF:
    case TargetThread::Method::EXTDEF_ZERO_DISPLACE: {
      const auto& extName = unit.imports[fixup.targetThread.index - 1];
      std::optional<GlobalName> name = globalNames.lookupName(extName.name);
      if (!name.has_value()) {
        // std::string error = std::format("Unresolved symbol: {}",
        // extName.name);
        throw std::runtime_error{"Unresolved symbol"};
      }
      return name->segment.baseAddress + name->offset +
          fixup.targetDisplacement;
    }

    default:
      throw std::runtime_error{"Unsupported fixup target method"};
  }
}

} // namespace

void writeTranslationUnit(
    uint8_t* outBuffer,
    const NameSet& globalNames,
    const TranslationUnit& unit) {
  for (const auto& segment : unit.segments) {
    for (const auto& dataBlock : segment.dataBlocks) {
      uint16_t currentBase = segment.baseAddress + dataBlock.dataOffset;
      std::memcpy(
          outBuffer + currentBase,
          dataBlock.data.data(),
          dataBlock.data.size());

      for (const auto& fixup : dataBlock.fixups) {
        if (fixup.frameThread.method != FrameThread::Method::TARG_SEG) {
          throw std::runtime_error{"Unsupported fixup frame thread"};
        }

        if (fixup.locationType != FixupData::LocationType::OFFSET_16BIT &&
            fixup.locationType != FixupData::LocationType::OFFSET_32BIT) {
          throw std::runtime_error{"Unsupported fixup location type"};
        }

        uint16_t targetAddress =
            getTargetAddress(fixup, globalNames, currentBase, unit);
        uint16_t sourceAddress = currentBase + fixup.dataRecordOffset;
        uint16_t sourceData =
            *reinterpret_cast<uint16_t*>(outBuffer + sourceAddress);

        switch (fixup.relativeTo) {
          case FixupData::RelativeTo::SEGMENT_RELATIVE:
            break;
          case FixupData::RelativeTo::SELF_RELATIVE:
            targetAddress -= sourceAddress + 2;
            break;
          default:
            throw std::runtime_error{"Invalid relativeness"};
        }

        *reinterpret_cast<uint16_t*>(outBuffer + sourceAddress) =
            targetAddress + sourceData;

        if (fixup.locationType == FixupData::LocationType::OFFSET_32BIT) {
          // *reinterpret_cast<uint16_t*>(outBuffer + sourceAddress + 2) = 0;
        }
      }
    }
  }
}

} // namespace omf
