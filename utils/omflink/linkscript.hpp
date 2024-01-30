#pragma once

#include <cstdint>
#include <string>
#include <variant>
#include <vector>

namespace omf {

struct LinkScriptAddress {
  uint16_t address;
};

struct LinkScriptSection {
  std::string name;
};

using LinkScriptItem = std::variant<LinkScriptAddress, LinkScriptSection>;
using LinkScript = std::vector<LinkScriptItem>;

LinkScript loadLinkScript(const char* filename);

} // namespace omf