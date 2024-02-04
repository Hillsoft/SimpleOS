#include <cstring>
#include <fstream>
#include <iostream>
#include <memory>

struct __attribute__((packed)) BootSector {
  uint8_t jumpInstruction[3];
  uint8_t oemIdentifier[8];
  uint16_t bytesPerSector;
  uint8_t sectorsPerCluster;
  uint16_t reservedSectors;
  uint8_t fatCount;
  uint16_t dirEntriesCount;
  uint16_t totalSectors;
  uint8_t mediaDescriptorType;
  uint16_t sectorsPerFat;
  uint16_t sectorsPerTrack;
  uint16_t heads;
  uint32_t hiddenSectors;
  uint32_t largeSectorCount;

  // extended boot record
  uint8_t driveNumber;
  uint8_t reserved;
  uint8_t signature;
  uint32_t volumeId;
  uint8_t volumeLabel[11];
  uint8_t system_id[8];
};

struct __attribute__((packed)) DirectoryEntry {
  uint8_t name[11];
  uint8_t attributes;
  uint8_t reserved;
  uint8_t createdTimeTenths;
  uint16_t createdTime;
  uint16_t createdDate;
  uint16_t accessedDate;
  uint16_t firstClusterHigh;
  uint16_t modifiedTime;
  uint16_t modifiedDate;
  uint16_t firstClusterLow;
  uint32_t size;
};

BootSector g_bootSector;
std::unique_ptr<unsigned char[]> g_Fat;
std::unique_ptr<DirectoryEntry[]> g_rootDirectory;
uint32_t g_rootDirectoryEnd;

bool readBootSector(std::istream& disk) {
  disk.seekg(0);
  disk.read(reinterpret_cast<char*>(&g_bootSector), sizeof(g_bootSector));
  return !disk.fail();
}

bool readSectors(std::istream& disk, uint32_t lba, uint32_t count, unsigned char* bufferOut) {
  bool ok = true;
  ok = ok && !disk.seekg(lba * g_bootSector.bytesPerSector).fail();
  ok = ok && !disk.read(reinterpret_cast<char*>(bufferOut), g_bootSector.bytesPerSector * count).fail();
  return ok;
}

bool readFat(std::ifstream& disk) {
  g_Fat = std::make_unique_for_overwrite<unsigned char[]>(g_bootSector.sectorsPerFat * g_bootSector.bytesPerSector);
  return readSectors(disk, g_bootSector.reservedSectors, g_bootSector.sectorsPerFat, g_Fat.get());
}

bool readRootDirectory(std::ifstream& disk) {
  uint32_t lba = g_bootSector.reservedSectors + g_bootSector.sectorsPerFat * g_bootSector.fatCount;
  uint32_t size = sizeof(DirectoryEntry) * g_bootSector.dirEntriesCount;
  uint32_t sectors = (size + g_bootSector.bytesPerSector - 1) / g_bootSector.bytesPerSector;

  // g_rootDirectory = (DirectoryEntry*)malloc(sectors * g_bootSector.bytesPerSector);
  uint32_t numEntries = (sizeof(DirectoryEntry) - 1 + sectors * g_bootSector.bytesPerSector) / sizeof(DirectoryEntry);
  g_rootDirectory = std::make_unique_for_overwrite<DirectoryEntry[]>(numEntries);

  g_rootDirectoryEnd = lba + sectors;

  return readSectors(disk, lba, sectors, reinterpret_cast<unsigned char*>(g_rootDirectory.get()));
}

DirectoryEntry* findFile(const char* name) {
  for (uint32_t i = 0; i < g_bootSector.dirEntriesCount; i++) {
    if (std::memcmp(name, g_rootDirectory[i].name, 11) == 0) {
      return &g_rootDirectory[i];
    }
  }
  return nullptr;
}

bool readFile(std::ifstream& disk, DirectoryEntry* fileEntry, unsigned char* outputBuffer) {
  bool ok = true;
  uint16_t currentCluster = fileEntry->firstClusterLow;

  do {
    uint32_t lba = g_rootDirectoryEnd + (currentCluster - 2) * g_bootSector.sectorsPerCluster;
    ok = ok && readSectors(disk, lba, g_bootSector.sectorsPerCluster, outputBuffer);
    outputBuffer += g_bootSector.sectorsPerCluster * g_bootSector.bytesPerSector;

    uint32_t fatIndex = currentCluster * 3 / 2;
    if (currentCluster % 2 == 0) {
      currentCluster = *reinterpret_cast<uint16_t*>(&g_Fat[fatIndex]) & 0x0FFF;
    }
    else {
      currentCluster = *reinterpret_cast<uint16_t*>(&g_Fat[fatIndex]) >> 4;
    }
  } while (ok && currentCluster < 0x0FF8);

  return ok;
}

int main(int argc, char** argv) {
  if (argc < 3) {
    std::cout << "Syntax: " << argv[0] << " <disk image> <file name>" << std::endl;
    return -1;
  }

  std::ifstream disk{argv[1], std::ios::in | std::ios::binary};
  if (!disk.is_open()) {
    std::cerr << "Cannot open disk image " << argv[1] << "!" << std::endl;
    return -1;
  }

  if (!readBootSector(disk)) {
    std::cerr <<  "Could not read boot sector!" << std::endl;
    return -2;
  }

  if (!readFat(disk)) {
    std::cerr << "Could not read FAT!" << std::endl;
    return -3;
  }

  if (!readRootDirectory(disk)) {
    std::cerr << "Could not read root directory!" << std::endl;
    return -4;
  }

  DirectoryEntry* fileEntry = findFile(argv[2]);
  if (fileEntry == nullptr) {
    std::cerr << "Could not find file " << argv[2] << "!" << std::endl;
    return -5;
  }

  std::unique_ptr<unsigned char[]> buffer = std::make_unique_for_overwrite<unsigned char[]>(fileEntry->size + g_bootSector.bytesPerSector);
  if (!readFile(disk, fileEntry, buffer.get())) {
    std::cerr << "Could not read file " << argv[2] << "!" << std::endl;
  }

  std::cout << "Read " << fileEntry->size << " bytes" << std::endl;

  for (size_t i = 0; i < fileEntry->size; i++) {
    if (isprint(buffer[i])) {
      std::cout << buffer[i];
    }
    else {
      std::printf("<%02x>", buffer[i]);
    }
  }
  std::cout << std::endl;

  return 0;
}
