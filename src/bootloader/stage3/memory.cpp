#include "memory.hpp"

#include "memlayout.hpp"
#include "mysty/int.hpp"
#include "mysty/io.hpp"

namespace simpleos {

namespace {

struct __attribute__((packed)) MemMapEntry {
  uint64_t base;
  uint64_t length;
  uint32_t type;
  uint32_t acpiAttributes;
};

} // namespace

void initializeMemory() {
  uint32_t memMapBytes = *reinterpret_cast<uint32_t*>(kMemMapSize);
  uint32_t memMapSize = memMapBytes / sizeof(MemMapEntry);
  mysty::printf("Entries in memory map: %u\n", memMapSize);

  MemMapEntry* entry = reinterpret_cast<MemMapEntry*>(kMemMapStart);
  for (size_t i = 0; i < memMapSize; i++) {
    mysty::printf(
        "Start: %X, Length: %X, Type: %u\n",
        entry[i].base,
        entry[i].length,
        entry[i].type);
  }
}

} // namespace simpleos
