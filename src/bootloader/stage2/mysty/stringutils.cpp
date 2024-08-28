#include "mysty/stringutils.hpp"

namespace mysty {

mysty::Vector<mysty::StringView> splitStringView(
    mysty::StringView str, char delimiter) {
  size_t delimiterCount = 0;
  for (char c : str) {
    if (c == delimiter) {
      delimiterCount++;
    }
  }

  mysty::Vector<mysty::StringView> result;
  result.reserve(delimiterCount + 1);

  const char* chunkStart = str.get(0);
  for (size_t i = 0; i < str.size(); i++) {
    if (str[i] == delimiter) {
      const char* chunkEnd = str.get(i);
      result.emplace_back(chunkStart, chunkEnd);
      chunkStart = chunkEnd + 1;
    }
  }
  result.emplace_back(chunkStart, str.get(str.size()));

  return result;
}

} // namespace mysty
