#include "interrupts.hpp"

#include "mysty/array.hpp"
#include "mysty/int.hpp"
#include "x86.hpp"

namespace simpleos {

namespace {

constexpr uint16_t kPic1Command = 0x20;
constexpr uint16_t kPic1Data = 0x21;
constexpr uint16_t kPic2Command = 0xA0;
constexpr uint16_t kPic2Data = 0xA1;

void suspendInterrupts() {
  asm volatile("cli");
}

void restoreInterrupts() {
  asm volatile("sti");
}

struct __attribute__((packed)) InterruptDescriptor32 {
  uint16_t offsetLo;
  uint16_t selector;
  uint8_t zero;
  uint8_t typeAttributes;
  uint16_t offsetHi;

  static InterruptDescriptor32 make(void* handler, InterruptType type) {
    uint32_t address =
        static_cast<uint32_t>(reinterpret_cast<uint64_t>(handler));
    uint8_t gateType = type == InterruptType::Interrupt ? 0xE : 0xF;
    uint8_t attributes = 0x80 | gateType;
    return InterruptDescriptor32{
        .offsetLo = static_cast<uint16_t>(address & 0xFFFF),
        .selector = 0x8,
        .zero = 0,
        .typeAttributes = attributes,
        .offsetHi = static_cast<uint16_t>(address >> 16)};
  }
};

__attribute__((
    aligned(0x10))) constinit mysty::FixedArray<InterruptDescriptor32, 256>
    interruptTable{InterruptDescriptor32{
        .offsetLo = 0x0,
        .selector = 0x0,
        .zero = 0,
        .typeAttributes = 0x0,
        .offsetHi = 0x0}};

struct __attribute__((packed)) InterruptDescriptorTable {
  uint16_t limit;
  uint32_t address;
};

InterruptDescriptorTable tableDefinition;

void initializePIC() {
  uint8_t a1 = x86_inb(kPic1Data);
  uint8_t a2 = x86_inb(kPic2Data);

  // start initialisation
  x86_outb(kPic1Command, 0x11);
  x86_io_wait();
  x86_outb(kPic2Command, 0x11);
  x86_io_wait();

  // vector offsets
  x86_outb(kPic1Data, 0x20);
  x86_io_wait();
  x86_outb(kPic2Data, 0x28);
  x86_io_wait();

  // tell PICs about each other
  x86_outb(kPic1Data, 4);
  x86_io_wait();
  x86_outb(kPic2Data, 2);
  x86_io_wait();

  // use 8086 mode
  x86_outb(kPic1Data, 0x01);
  x86_io_wait();
  x86_outb(kPic2Data, 0x01);
  x86_io_wait();

  x86_outb(kPic1Data, a1);
  x86_outb(kPic2Data, a2);
}

} // namespace

void initializeInterrupts() {
  initializePIC();

  for (auto& table : interruptTable) {
    table = InterruptDescriptor32::make(
        reinterpret_cast<void*>(x86_trivial_interrupt),
        InterruptType::Interrupt);
  }

  tableDefinition = {
      .limit = sizeof(interruptTable),
      .address = static_cast<uint32_t>(
          reinterpret_cast<uint64_t>(&interruptTable[0]))};

  x86_load_interrupt_table(&tableDefinition);
  restoreInterrupts();
}

void registerInterrupt(
    uint8_t offset,
    __attribute__((interrupt)) void (*handler)(void*),
    InterruptType type,
    InterruptRange range) {
  suspendInterrupts();

  switch (range) {
    case InterruptRange::PIC:
      offset += 0x20;
    case InterruptRange::CPU:
    default:
      break;
  }

  interruptTable[offset] =
      InterruptDescriptor32::make(reinterpret_cast<void*>(handler), type);

  restoreInterrupts();
}

} // namespace simpleos
