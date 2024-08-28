#pragma once

#include "mysty/span.hpp"
#include "mysty/vector.hpp"

namespace mysty {

mysty::Vector<mysty::StringView> splitStringView(
    mysty::StringView str, char delimiter);

} // namespace mysty
