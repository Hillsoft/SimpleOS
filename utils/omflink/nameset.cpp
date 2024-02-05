#include "nameset.hpp"

// #include <format>

namespace omf {

NameSet::NameSet() {}

NameSet::NameSet(const std::vector<TranslationUnit>& units) {
  for (const auto& u : units) {
    registerTranslationUnit(u);
  }
}

void NameSet::registerTranslationUnit(const TranslationUnit& unit) {
  for (const auto& n : unit.exports) {
    if (names.contains(n.name)) {
      // std::string error = std::format("Duplicate symbol {}", n.name);
      // throw std::runtime_error{error};
    }

    names.emplace(
        std::piecewise_construct,
        std::forward_as_tuple(n.name),
        std::forward_as_tuple(unit.segments[n.segmentIndex - 1], n.offset));
  }
}

std::optional<GlobalName> NameSet::lookupName(std::string_view name) const {
  auto it = names.find(name);

  if (it == names.end()) {
    return std::nullopt;
  }

  return it->second;
}

} // namespace omf
