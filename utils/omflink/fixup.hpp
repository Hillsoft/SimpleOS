#pragma once

#include "nameset.hpp"
#include "omfunit.hpp"

namespace omf {

void writeTranslationUnit(uint8_t* outBuffer, const NameSet& globalNames, const TranslationUnit& unit);

} // namespace omf
