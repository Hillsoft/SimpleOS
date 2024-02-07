#include "disk.hpp"

#include "interrupts.hpp"
#include "mysty/array.hpp"
#include "mysty/concepts/iterable.hpp"
#include "mysty/io.hpp"
#include "mysty/optional.hpp"
#include "x86.hpp"

namespace simpleos::disk {

namespace {

constinit volatile bool g_hasIRQ6 = false;
constinit bool g_hasConfiguredDriveController = false;
constinit uint8_t g_currentDrive = 0;

constexpr uint32_t kHeadsPerCylinder = 2;
constexpr uint32_t kSectorsPerTrack = 18;

struct CHS {
  uint16_t cylinder;
  uint16_t head;
  uint16_t sector;

  static constexpr CHS fromLBA(uint32_t lba) {
    return CHS{
        .cylinder =
            static_cast<uint16_t>((lba / kSectorsPerTrack) / kHeadsPerCylinder),
        .head =
            static_cast<uint16_t>((lba / kSectorsPerTrack) % kHeadsPerCylinder),
        .sector = static_cast<uint16_t>((lba % kSectorsPerTrack) + 1)};
  }
};

enum FloppyRegister : uint16_t {
  STATUS_REGISTER_A = 0x3F0, // read-only
  STATUS_REGISTER_B = 0x3F1, // read-only
  DIGITAL_OUTPUT_REGISTER = 0x3F2,
  TAPE_DRIVE_REGISTER = 0x3F3,
  MAIN_STATUS_REGISTER = 0x3F4, // read-only
  DATARATE_SELECT_REGISTER = 0x3F4, // write-only
  DATA_FIFO = 0x3F5,
  DIGITAL_INPUT_REGISTER = 0x3F7, // read-only
  CONFIGURATION_CONTROL_REGISTER = 0x3F7, // write-only
};

enum FloppyCommand : uint8_t {
  READ_TRACK = 2,
  SPECIFY = 3,
  SENSE_DRIVE_STATUS = 4,
  WRITE_DATA = 5,
  READ_DATA = 6,
  RECALIBRATE = 7,
  SENSE_INTERRUPT = 8,
  WRITE_DELETED_DATA = 9,
  READ_ID = 10,
  READ_DELETED_DATA = 12,
  FORMAT_TRACK = 13,
  DUMPREG = 14,
  SEEK = 15,
  VERSION = 16,
  SCAN_EQUAL = 17,
  PERPENDICULAR_MODE = 18,
  CONFIGURE = 19,
  LOCK = 20,
  VERIFY = 22,
  SCAN_LOW_OR_EQUAL = 25,
  SCAN_HIGH_OR_EQUAL = 29,
};

class MainStatusRegisterBitset {
 public:
  explicit MainStatusRegisterBitset(uint8_t bitset) : bitset_(bitset) {}

  static MainStatusRegisterBitset loadFromController() {
    return MainStatusRegisterBitset{
        x86_inb(FloppyRegister::MAIN_STATUS_REGISTER)};
  }

  constexpr bool getRQM() const { return (bitset_ & 0x80) > 0; }

  constexpr bool getDIO() const { return (bitset_ & 0x40) > 0; }

  constexpr bool getNDMA() const { return (bitset_ & 0x20) > 0; }

  constexpr bool getCB() const { return (bitset_ & 0x10) > 0; }

  constexpr bool isDriveSeeking(uint8_t driveNumber) {
    return (bitset_ & (0x1 << driveNumber)) > 0;
  }

 private:
  uint8_t bitset_;
};

void selectDrive(uint8_t driveNumber) {
  x86_outb(FloppyRegister::CONFIGURATION_CONTROL_REGISTER, 0);

  uint8_t motorOnBit = 0x10 << driveNumber;
  uint8_t setBit = 0x4;
  uint8_t selectBits = driveNumber;

  x86_outb(
      FloppyRegister::DIGITAL_OUTPUT_REGISTER,
      motorOnBit | setBit | selectBits);
}

void resetDriveController() {
  x86_outb(FloppyRegister::DATARATE_SELECT_REGISTER, 0x80);

  // Assume a LOCK command has been issued earlier

  selectDrive(g_currentDrive);
}

template <
    mysty::ConstIterable<uint8_t> InputContainer,
    mysty::Iterable<uint8_t> OutputContainer,
    mysty::Iterable<uint8_t> BufferContainer>
bool executeDriveControllerCommandSingleAttempt(
    FloppyCommand command,
    const InputContainer& inputParameters,
    OutputContainer& outBytes,
    BufferContainer& buffer,
    size_t& bytesReadToBuffer) {
  bytesReadToBuffer = 0;

  MainStatusRegisterBitset status =
      MainStatusRegisterBitset::loadFromController();

  if (!status.getRQM() || status.getDIO()) {
    resetDriveController();
    return false;
  }

  x86_outb(FloppyRegister::DATA_FIFO, command);

  for (const auto& parameter : inputParameters) {
    // Wait until ready
    while (status = MainStatusRegisterBitset::loadFromController(),
           !status.getRQM()) {
    }
    if (status.getDIO()) {
      return false;
    }

    x86_outb(FloppyRegister::DATA_FIFO, parameter);
  }

  // Check if the current command has an execution phase
  status = MainStatusRegisterBitset::loadFromController();
  auto bufferIt = buffer.begin();
  while (status.getNDMA() && bufferIt != buffer.end()) {
    // We do have an execution phase
    while (status = MainStatusRegisterBitset::loadFromController(),
           !status.getRQM()) {
    }

    while (status = MainStatusRegisterBitset::loadFromController(),
           status.getRQM() && status.getNDMA()) {
      uint8_t curByte = x86_inb(FloppyRegister::DATA_FIFO);
      if (bufferIt != buffer.end()) {
        *bufferIt = curByte;
        ++bufferIt;
        ++bytesReadToBuffer;
      }
    }
  }

  // Await execution complete
  while (status = MainStatusRegisterBitset::loadFromController(),
         status.getNDMA()) {
  }

  // Result phase
  if (outBytes.size() > 0) {
    while (status = MainStatusRegisterBitset::loadFromController(),
           !status.getRQM()) {
    }
    if (!status.getDIO()) {
      return false;
    }

    for (auto& outByte : outBytes) {
      while (status = MainStatusRegisterBitset::loadFromController(),
             !status.getRQM()) {
      }
      if (!status.getDIO() || !status.getCB()) {
        return false;
      }
      outByte = x86_inb(FloppyRegister::DATA_FIFO);
    }
  }

  status = MainStatusRegisterBitset::loadFromController();
  if (!status.getRQM() || status.getCB() || status.getDIO()) {
    return false;
  }

  return true;
}

template <
    mysty::ConstIterable<uint8_t> InputContainer,
    mysty::Iterable<uint8_t> OutputContainer,
    mysty::Iterable<uint8_t> BufferContainer>
bool executeDriveControllerCommand(
    FloppyCommand command,
    const InputContainer& inputParameters,
    OutputContainer& outBytes,
    BufferContainer& buffer,
    size_t& bytesReadToBuffer) {
  // Try up to three times
  for (int i = 0; i < 3; i++) {
    if (executeDriveControllerCommandSingleAttempt(
            command, inputParameters, outBytes, buffer, bytesReadToBuffer)) {
      return true;
    }
  }

  return false;
}

template <
    mysty::ConstIterable<uint8_t> InputContainer,
    mysty::Iterable<uint8_t> OutputContainer,
    mysty::Iterable<uint8_t> BufferContainer>
bool executeDriveControllerCommand(
    FloppyCommand command,
    const InputContainer& inputParameters,
    OutputContainer& outBytes,
    BufferContainer& buffer) {
  size_t bytesReadToBuffer;
  // Try up to three times
  for (int i = 0; i < 3; i++) {
    if (executeDriveControllerCommandSingleAttempt(
            command, inputParameters, outBytes, buffer, bytesReadToBuffer)) {
      return true;
    }
  }

  return false;
}

mysty::Optional<uint8_t> getDriveControllerVersion() {
  mysty::FixedArray<uint8_t, 0> input;
  mysty::FixedArray<uint8_t, 1> output;
  mysty::FixedArray<uint8_t, 0> buffer;

  bool success = executeDriveControllerCommand(
      FloppyCommand::VERSION, input, output, buffer);

  if (success) {
    return output[0];
  }

  return mysty::Optional<uint8_t>{};
}

bool configureDriveController(
    bool impliedSeekEnable,
    bool fifoEnable,
    bool pollingEnable,
    uint8_t threshold,
    uint8_t precompensation) {
  mysty::FixedArray<uint8_t, 3> input{
      0,
      (impliedSeekEnable << 6) | (!fifoEnable << 5) | (!pollingEnable << 4) |
          threshold,
      precompensation};
  mysty::FixedArray<uint8_t, 0> output;
  mysty::FixedArray<uint8_t, 0> buffer;

  return executeDriveControllerCommand(
      FloppyCommand::CONFIGURE, input, output, buffer);
}

bool unlockDriveController() {
  mysty::FixedArray<uint8_t, 0> input;
  mysty::FixedArray<uint8_t, 1> output;
  mysty::FixedArray<uint8_t, 0> buffer;

  if (!executeDriveControllerCommand(
          FloppyCommand::LOCK, input, output, buffer)) {
    return false;
  }

  return output[0] == 0;
}

bool lockDriveController() {
  mysty::FixedArray<uint8_t, 0> input;
  mysty::FixedArray<uint8_t, 1> output;
  mysty::FixedArray<uint8_t, 0> buffer;

  if (!executeDriveControllerCommand(
          static_cast<FloppyCommand>(0x80u | FloppyCommand::LOCK),
          input,
          output,
          buffer)) {
    return false;
  }

  return output[0] == (0x1 << 4);
}

bool recalibrate() {
  mysty::FixedArray<uint8_t, 1> input{g_currentDrive};
  mysty::FixedArray<uint8_t, 0> output;
  mysty::FixedArray<uint8_t, 0> buffer;

  g_hasIRQ6 = false;

  if (!executeDriveControllerCommand(
          FloppyCommand::RECALIBRATE, input, output, buffer)) {
    return false;
  }

  // Poll until head movement is finished
  while (MainStatusRegisterBitset::loadFromController().isDriveSeeking(
      g_currentDrive)) {
  }

  while (!g_hasIRQ6) {
  }

  mysty::FixedArray<uint8_t, 0> senseInput;
  mysty::FixedArray<uint8_t, 2> senseOutput;
  mysty::FixedArray<uint8_t, 0> senseBuffer;

  if (!executeDriveControllerCommand(
          FloppyCommand::SENSE_INTERRUPT,
          senseInput,
          senseOutput,
          senseBuffer)) {
    return false;
  }

  if (senseOutput[0] != (0x20 | g_currentDrive)) {
    return false;
  }

  if (senseOutput[1] != 0) {
    return false;
  }

  return true;
}

} // namespace

bool initialize(uint8_t driveNumber) {
  registerInterrupt(6, floppyInterruptHandlerWrapper, InterruptType::Interrupt);

  // We will reset later which sends this to the controller
  g_currentDrive = driveNumber;

  mysty::Optional<uint8_t> version = getDriveControllerVersion();
  if (!version.has_value()) {
    mysty::puts("Failed to get floppy controller version\n");
    return false;
  }
  if (*version != 0x90) {
    mysty::puts("Unsupported floppy controller\n");
    return false;
  }

  unlockDriveController();

  if (!configureDriveController(true, true, false, 8, 0)) {
    mysty::puts("Failed to configure floppy controller\n");
    return false;
  }

  if (!lockDriveController()) {
    mysty::puts("Failed to lock floppy controller\n");
    return false;
  }

  resetDriveController();

  if (!recalibrate()) {
    mysty::puts("Failed to recalibrate floppy\n");
    return false;
  }

  return true;
}

size_t read(uint32_t lba, mysty::Span<uint8_t> outBuffer) {
  CHS chsAddress = CHS::fromLBA(lba);

  mysty::FixedArray<uint8_t, 8> input{
      (chsAddress.head << 2) | g_currentDrive,
      chsAddress.cylinder,
      chsAddress.head,
      chsAddress.sector,
      2,
      kSectorsPerTrack,
      0x1b,
      0xff};
  mysty::FixedArray<uint8_t, 7> output;

  size_t bytesRead;
  if (!executeDriveControllerCommand(
          static_cast<FloppyCommand>(0x80 | 0x40 | FloppyCommand::READ_DATA),
          input,
          output,
          outBuffer,
          bytesRead)) {
    return 0;
  }

  if ((output[0] & 0xC0) > 0) {
    return 0;
  }
  if (output[1] > 0) {
    return 0;
  }
  if (output[2] > 0) {
    return 0;
  }

  return bytesRead;
}

extern "C" {
__attribute__((cdecl)) void floppyInterruptHandler() {
  g_hasIRQ6 = true;
}
}

} // namespace simpleos::disk
