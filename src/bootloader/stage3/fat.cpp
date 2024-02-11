#include "fat.hpp"

#include "disk.hpp"
#include "memlayout.hpp"
#include "mysty/array.hpp"
#include "mysty/int.hpp"
#include "mysty/io.hpp"
#include "mysty/new.hpp"
#include "mysty/span.hpp"

namespace simpleos {

namespace {

constexpr size_t kBytesPerSector = 512;
constexpr size_t kMaxOpenFiles = 10;

struct FileData {
  bool isOpen;
  bool isDirectory;
  uint32_t position;
  uint32_t size;
  uint32_t firstCluster;
  uint32_t currentCluster;
  uint32_t currentSectorInCluster;
  mysty::FixedArray<uint8_t, kBytesPerSector> buffer;
};

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
  char name[11];
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
constinit mysty::Span<uint8_t> g_fat;
mysty::FixedArray<FileData, kMaxOpenFiles>* g_openFiles;
uint32_t g_rootDirectoryEnd;

bool readBootSector() {
  mysty::Span<uint8_t> bootSectorBytes{
      reinterpret_cast<uint8_t*>(&g_bootSector), sizeof(BootSector)};
  return disk::read(0, bootSectorBytes) == sizeof(BootSector);
}

bool readFAT() {
  uint8_t* fatStart = reinterpret_cast<uint8_t*>(kFatArenaStart);
  size_t fatSize = g_bootSector.sectorsPerFat * kBytesPerSector;
  uint8_t* fatEnd = fatStart + fatSize;

  if (fatEnd >= kFatArenaEnd) {
    return false;
  }

  g_fat = mysty::Span<uint8_t>{fatStart, fatEnd};

  return disk::read(g_bootSector.reservedSectors, g_fat) == fatSize;
}

bool initializeFileDataStructure() {
  uint8_t* openFilesStart =
      reinterpret_cast<uint8_t*>(kFatArenaStart) + g_fat.size();
  uint8_t* openFilesEnd = openFilesStart + sizeof(g_openFiles);

  if (openFilesEnd >= kFatArenaEnd) {
    return false;
  }

  g_openFiles = new (reinterpret_cast<void*>(openFilesStart))
      mysty::FixedArray<FileData, kMaxOpenFiles>(FileData{
          .isOpen = false,
          .isDirectory = false,
          .position = 0,
          .size = 0,
          .firstCluster = 0,
          .currentCluster = 0,
          .currentSectorInCluster = 0,
          .buffer = {}});

  return true;
}

bool readRootDirectory() {
  uint32_t lba = static_cast<uint32_t>(
      g_bootSector.reservedSectors +
      g_bootSector.sectorsPerFat * g_bootSector.fatCount);
  uint32_t size = sizeof(DirectoryEntry) * g_bootSector.dirEntriesCount;

  if (disk::read(lba, g_openFiles->at(0).buffer) != kBytesPerSector) {
    return false;
  }

  g_openFiles->at(0).isOpen = true;
  g_openFiles->at(0).isDirectory = true;
  g_openFiles->at(0).position = 0;
  g_openFiles->at(0).size = size;
  g_openFiles->at(0).firstCluster = lba;
  g_openFiles->at(0).currentCluster = lba;
  g_openFiles->at(0).currentSectorInCluster = 0;

  g_rootDirectoryEnd = lba + (size + kBytesPerSector - 1) / kBytesPerSector;

  return true;
}

DirectoryEntry* findFileInRootDirectory(mysty::StringView fileName) {
  // TODO: this assumes the root directory fits in a single sector, which is not
  // necessarily the case
  DirectoryEntry* rootDirectoryData =
      reinterpret_cast<DirectoryEntry*>(&g_openFiles->at(0).buffer[0]);
  for (uint32_t i = 0; i < g_bootSector.dirEntriesCount; ++i) {
    if (mysty::StringView{
            rootDirectoryData[i].name, sizeof(rootDirectoryData[i].name)} ==
        fileName) {
      return rootDirectoryData + i;
    }
  }

  return nullptr;
}

uint32_t clusterToLBA(uint32_t cluster) {
  return g_rootDirectoryEnd + (cluster - 2) * g_bootSector.sectorsPerCluster;
}

} // namespace

bool initializeFileSystem() {
  if (!readBootSector()) {
    mysty::puts("Failed to read boot sector\n");
    return false;
  }

  if (!readFAT()) {
    mysty::puts("Failed to read FAT\n");
    return false;
  }

  if (!initializeFileDataStructure()) {
    mysty::puts("Failed to initialise FAT data structure\n");
    return false;
  }

  if (!readRootDirectory()) {
    mysty::puts("Failed to load root directory\n");
    return false;
  }

  return true;
}

mysty::Optional<File> openFile(mysty::StringView path) {
  // For now, we assume the file is in the root directory
  // TODO: support subdirectories
  if (path.size() == 0) {
    return {};
  }

  if (path[0] == '/') {
    path.slice_front(1);
  }

  if (path.size() > 11 || path.size() < 4 || path.at(path.size() - 4) != '.') {
    return {};
  }

  uint8_t handle = 1; // 0 is always open
  for (; handle < g_openFiles->size(); ++handle) {
    if (!g_openFiles->at(handle).isOpen) {
      break;
    }
  }
  if (handle == g_openFiles->size()) {
    return {};
  }
  FileData& fileData = g_openFiles->at(handle);

  mysty::FixedArray<char, 11> fatFileName{' '};

  for (size_t i = 0; i < path.size() - 4; i++) {
    fatFileName[i] = path[i];
  }
  for (size_t i = 1; i <= 3; i++) {
    fatFileName[fatFileName.size() - i] = path[path.size() - i];
  }

  DirectoryEntry* fileEntry = findFileInRootDirectory(fatFileName);
  if (fileEntry == nullptr) {
    return {};
  }

  fileData.isDirectory = true;
  fileData.position = 0;
  fileData.size = fileEntry->size;
  fileData.firstCluster =
      (static_cast<uint32_t>(fileEntry->firstClusterHigh) << 16u) +
      fileEntry->firstClusterLow;
  fileData.currentCluster = fileData.firstCluster;
  fileData.currentSectorInCluster = 0;

  if (disk::read(clusterToLBA(fileData.currentCluster), fileData.buffer) !=
      kBytesPerSector) {
    return {};
  }

  fileData.isOpen = true;

  return File{handle};
}

File::File(uint8_t handle) : handle_(handle) {}

} // namespace simpleos
