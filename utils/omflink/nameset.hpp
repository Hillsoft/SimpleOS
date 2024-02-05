#pragma once

#include "omfunit.hpp"

#include <unordered_map>

namespace omf {

struct GlobalName {
  const SegmentDefinition& segment;
  uint16_t offset;
};

class NameSet final {
 public:
  NameSet();
  NameSet(const std::vector<TranslationUnit>& units);

  void registerTranslationUnit(const TranslationUnit& unit);

  std::optional<GlobalName> lookupName(std::string_view name) const;

 private:
  std::unordered_map<std::string_view, GlobalName> names;
};

} // namespace omf
