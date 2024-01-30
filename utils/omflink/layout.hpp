#pragma once

#include <vector>

#include "omfunit.hpp"

namespace omf {

// Orders segments and returns the ordered list
// Also updates the baseAddress field on the underlying SegmentDefinition objects
std::vector<SegmentDefinition*> arrangeSegments(std::vector<TranslationUnit>& units);

} // namespace omf
