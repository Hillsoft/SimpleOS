#pragma once

namespace simpleos {

inline void* kMemMapSize = reinterpret_cast<void*>(0x500);
inline void* kMemMapStart = reinterpret_cast<void*>(0x508);
inline void* kFatArenaStart = reinterpret_cast<void*>(0x1000);
inline void* kFatArenaEnd = reinterpret_cast<void*>(0x11000);

} // namespace simpleos
