#include "linkscript.hpp"

// #include <format>
#include <fstream>
#include <stdexcept>

namespace omf {

std::vector<LinkScriptItem> loadLinkScript(const char* filename) {
  std::ifstream file{filename, std::ios::in};
  if (!file.is_open()) {
    // throw std::runtime_error{std::format("Failed to open link script: {}", filename)};
  }

  std::vector<LinkScriptItem> linkScript;

  std::string line;
  while (std::getline(file, line)) {
    if (line.starts_with("0x")) {
      if (line.size() > 6) {
        throw std::runtime_error{"Only 16-bit addresses are supported"};
      }

      char* p;
      uint16_t addr = std::strtol(line.data() + 2, &p, 16);
      if (*p != 0) {
        // throw std::runtime_error{std::format("Invalid address format: {}", line)};
      }

      linkScript.emplace_back(std::in_place_type_t<LinkScriptAddress>{}, LinkScriptAddress{addr});
    }
    else if (line.starts_with(".")) {
      // Validate this is not a duplicate
      std::string currentSection = line.substr(1);
      for (const auto& item : linkScript) {
        if (std::holds_alternative<LinkScriptSection>(item) && std::get<LinkScriptSection>(item).name == currentSection) {
          // throw std::runtime_error{std::format("Duplicate section: {}", currentSection)};
        }
      }

      linkScript.emplace_back(std::in_place_type_t<LinkScriptSection>{}, LinkScriptSection{std::move(currentSection)});
    }
    else if (line.size() > 0) {
      // throw std::runtime_error{std::format("Improperly formatted link script item: {}", line)};
    }
  }

  return linkScript;
}

} // namespace
