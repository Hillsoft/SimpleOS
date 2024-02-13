#include "memory.hpp"

#include "memlayout.hpp"
#include "mysty/io.hpp"

namespace simpleos {

namespace {

enum class MemoryEntryType : uint32_t {
  USABLE = 1,
  RESERVED = 2,
  ACPI_RECLAIMABLE = 3,
  ACPI_NVS = 4,
  BAD = 5,
};

struct __attribute__((packed)) MemMapEntry {
  uint64_t base;
  uint64_t length;
  MemoryEntryType type;
  uint32_t acpiAttributes;
};

struct ChunkEntry {
  ChunkEntry* prev; // this pointer will always be 4-aligned, so we use its
                    // lowest bit to store isUsed
  size_t size;
};

bool isUsed(ChunkEntry const& chunk) {
  return (reinterpret_cast<size_t>(chunk.prev) & 0b1) > 0;
}

constexpr size_t align(size_t x, size_t alignment) {
  if (x % alignment == 0) {
    return x;
  }
  return x + alignment - (x % alignment);
}

MemMapEntry mainArena;

} // namespace

bool initializeMemory() {
  uint32_t memMapBytes = *reinterpret_cast<uint32_t*>(kMemMapSize);
  uint32_t memMapSize = memMapBytes / sizeof(MemMapEntry);

  mainArena = {0, 0, MemoryEntryType::BAD, 0};

  MemMapEntry* entries = reinterpret_cast<MemMapEntry*>(kMemMapStart);
  for (size_t i = 0; i < memMapSize; i++) {
    if (entries[i].type == MemoryEntryType::USABLE &&
        entries[i].length > mainArena.length) {
      mainArena = entries[i];
    }
  }

  if (mainArena.type != MemoryEntryType::USABLE) {
    constexpr mysty::StringView errorMessage{"No suitable memory block\n"};
    mysty::puts(errorMessage);
    return false;
  }

  if (mainArena.length > int_limits<size_t>::max) {
    constexpr mysty::StringView errorMessage{"Memory block too large\n"};
    mysty::puts(errorMessage);
    return false;
  }

  ChunkEntry* firstEntry = reinterpret_cast<ChunkEntry*>(mainArena.base);
  firstEntry->prev = nullptr;
  firstEntry->size = mainArena.length - 2 * sizeof(ChunkEntry);

  ChunkEntry* lastEntry = reinterpret_cast<ChunkEntry*>(
      firstEntry->size + sizeof(ChunkEntry) +
      reinterpret_cast<uint8_t*>(firstEntry));
  lastEntry->prev = firstEntry;
  lastEntry->size = 0;

  return true;
}

void* malloc(size_t size) {
  size = align(size, sizeof(size_t));

  ChunkEntry* curEntry = reinterpret_cast<ChunkEntry*>(mainArena.base);
  while (isUsed(*curEntry)) {
    if (curEntry->size == 0) {
      return nullptr;
    }

    curEntry = reinterpret_cast<ChunkEntry*>(
        curEntry->size + sizeof(ChunkEntry) +
        reinterpret_cast<uint8_t*>(curEntry));
  }

  if (curEntry->size >= size + sizeof(ChunkEntry)) {
    // space to split
    ChunkEntry* newEntry = reinterpret_cast<ChunkEntry*>(
        size + sizeof(ChunkEntry) + reinterpret_cast<uint8_t*>(curEntry));
    newEntry->prev = curEntry;
    newEntry->size = curEntry->size - size - sizeof(ChunkEntry);

    curEntry->size = size;
  }

  // Mark as used
  curEntry->prev = reinterpret_cast<ChunkEntry*>(
      0b1 | reinterpret_cast<size_t>(curEntry->prev));

  return reinterpret_cast<uint8_t*>(curEntry) + sizeof(ChunkEntry);
}

void free(void* ptr) {
  ChunkEntry* curEntry = reinterpret_cast<ChunkEntry*>(
      reinterpret_cast<uint8_t*>(ptr) - sizeof(ChunkEntry));

  // Mark as unused
  curEntry->prev = reinterpret_cast<ChunkEntry*>(
      ~static_cast<size_t>(0b1) & reinterpret_cast<size_t>(curEntry->prev));

  // Merge with next and previous if they are free
  ChunkEntry* next = reinterpret_cast<ChunkEntry*>(
      curEntry->size + sizeof(ChunkEntry) +
      reinterpret_cast<uint8_t*>(curEntry));
  if (!isUsed(*next)) {
    curEntry->size += next->size + sizeof(ChunkEntry);
  }

  ChunkEntry* prev = curEntry->prev;
  if (!isUsed(*prev)) {
    prev->size += curEntry->size + sizeof(ChunkEntry);
  }
}

} // namespace simpleos

void* operator new(size_t size) {
  if (size == 0) {
    size = 1;
  }
  return simpleos::malloc(size);
}

void* operator new[](size_t size) {
  if (size == 0) {
    size = 1;
  }
  return simpleos::malloc(size);
}

void operator delete(void* ptr) noexcept {
  simpleos::free(ptr);
}

void operator delete[](void* ptr) noexcept {
  simpleos::free(ptr);
}

void operator delete(void* ptr, size_t size) noexcept {
  simpleos::free(ptr);
}

void operator delete[](void* ptr, size_t size) noexcept {
  simpleos::free(ptr);
}
