#include "fat.hpp"

#include "disk.hpp"
#include "memlayout.hpp"
#include "mysty/array.hpp"
#include "mysty/int.hpp"
#include "mysty/io.hpp"
#include "mysty/memory.hpp"
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

uint32_t clusterToLBA(uint32_t cluster) {
  return g_rootDirectoryEnd + (cluster - 2) * g_bootSector.sectorsPerCluster;
}

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

uint32_t advanceCluster(uint32_t cluster) {
  uint32_t fatIndex = cluster * 3 / 2;

  if (cluster % 2 == 0) {
    return *reinterpret_cast<uint16_t*>(&g_fat[fatIndex]) & 0x0FFF;
  } else {
    return *reinterpret_cast<uint16_t*>(&g_fat[fatIndex]) >> 4;
  }
}

bool advanceFileBuffer(uint8_t handle) {
  FileData& file = g_openFiles->at(handle);
  if (handle == 0) {
    // Special handling for the root directory
    ++file.currentCluster;
    return disk::read(file.currentCluster, file.buffer);
  } else {
    ++file.currentSectorInCluster;

    if (file.currentSectorInCluster >= g_bootSector.sectorsPerCluster) {
      file.currentSectorInCluster = 0;

      file.currentCluster = advanceCluster(file.currentCluster);
    }

    if (file.currentCluster >= 0x0FF8) {
      return false;
    }

    uint32_t lba =
        clusterToLBA(file.currentCluster) + file.currentSectorInCluster;

    return disk::read(lba, file.buffer);
  }
}

File::ReadResult readFile(uint8_t handle, mysty::Span<uint8_t> bufferOut) {
  if (bufferOut.size() == 0) {
    return File::ReadResult::OK;
  }

  FileData& file = g_openFiles->at(handle);
  size_t bytesToRead = mysty::min(
      static_cast<size_t>(file.size - file.position), bufferOut.size());

  {
    uint32_t positionInSector = file.position % kBytesPerSector;
    size_t currentBytesToRead = mysty::min(
        kBytesPerSector - static_cast<size_t>(positionInSector), bytesToRead);

    mysty::memcpy(
        bufferOut.begin(),
        file.buffer.begin() + positionInSector,
        currentBytesToRead);

    bufferOut = bufferOut.slice_front(currentBytesToRead);
    file.position += currentBytesToRead;
    bytesToRead -= currentBytesToRead;
  }

  while (bytesToRead > 0) {
    if (!advanceFileBuffer(handle)) {
      return File::ReadResult::FAILED;
    }

    size_t currentBytesToRead = mysty::min(kBytesPerSector, bytesToRead);

    mysty::memcpy(bufferOut.begin(), file.buffer.begin(), currentBytesToRead);

    bufferOut = bufferOut.slice_front(currentBytesToRead);
    file.position += currentBytesToRead;
    bytesToRead -= currentBytesToRead;
  }

  if (file.position % kBytesPerSector == 0 && file.position < file.size) {
    if (!advanceFileBuffer(handle)) {
      return File::ReadResult::FAILED;
    }
  }

  if (bufferOut.size() == 0) {
    return File::ReadResult::OK;
  } else {
    return File::ReadResult::REACHED_END;
  }
}

bool resetRootDirectory() {
  FileData& rootDirectory = g_openFiles->at(0);
  rootDirectory.position = 0;
  if (rootDirectory.currentCluster != rootDirectory.firstCluster) {
    rootDirectory.currentCluster = rootDirectory.firstCluster;
    return disk::read(rootDirectory.currentCluster, rootDirectory.buffer);
  }
  return true;
}

mysty::Optional<DirectoryEntry> findFileInRootDirectory(
    mysty::StringView fileName) {
  if (!resetRootDirectory()) {
    return {};
  }

  for (uint32_t i = 0; i < g_bootSector.dirEntriesCount; ++i) {
    DirectoryEntry currentEntry;
    readFile(
        0,
        mysty::Span<uint8_t>{
            reinterpret_cast<uint8_t*>(&currentEntry), sizeof(currentEntry)});
    if (mysty::StringView{currentEntry.name, sizeof(currentEntry.name)} ==
        fileName) {
      return currentEntry;
    }
  }

  return {};
}

void closeFile(uint8_t handle) {
  g_openFiles->at(handle).isOpen = false;
}

} // namespace

bool initializeFileSystem() {
  if (!readBootSector()) {
    constexpr mysty::StringView errorMessage{"Failed to read boot sector\n"};
    mysty::puts(errorMessage);
    return false;
  }

  if (!readFAT()) {
    mysty::puts("Failed to read FAT\n");
    return false;
  }

  if (!initializeFileDataStructure()) {
    constexpr mysty::StringView errorMessage{
        "Failed to initialise FAT data structure\n"};
    mysty::puts(errorMessage);
    return false;
  }

  if (!readRootDirectory()) {
    constexpr mysty::StringView errorMessage{"Failed to load root directory\n"};
    mysty::puts(errorMessage);
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

  mysty::Optional<DirectoryEntry> fileEntry =
      findFileInRootDirectory(fatFileName);
  if (!fileEntry.has_value()) {
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

File::~File() {
  close();
}

size_t File::remainingBytes() const {
  return size() - position();
}

size_t File::position() const {
  return g_openFiles->at(handle_).position;
}

size_t File::size() const {
  return g_openFiles->at(handle_).size;
}

File::ReadResult File::read(mysty::Span<uint8_t> bufferOut) {
  return readFile(handle_, bufferOut);
}

void File::close() {
  if (handle_ != 255) {
    closeFile(handle_);
    handle_ = 255;
  }
}

} // namespace simpleos
