#pragma once

#include <vector>

#include "linkscript.hpp"
#include "omfunit.hpp"

namespace omf {

// Orders segments and returns the ordered list
// Also updates the baseAddress field on the underlying SegmentDefinition objects
std::vector<SegmentDefinition*> arrangeSegments(const LinkScript& linkScript, std::vector<TranslationUnit>& units);

} // namespace omf
