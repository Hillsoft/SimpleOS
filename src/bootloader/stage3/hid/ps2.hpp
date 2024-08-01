#pragma once

#include "mysty/compat.hpp"
#include "mysty/optional.hpp"
#include "mysty/span.hpp"

namespace simpleos::hid {

enum class PS2DeviceType {
  MF2KEYBOARD,
};

enum class PS2Port { First, Second };

class PS2PortHandle {
 public:
  PS2Port getPort() const { return port_; }

 private:
  explicit PS2PortHandle(PS2Port port) : port_(port) {}

  PS2Port port_;

  friend mysty::Optional<PS2PortHandle> getPortForDevice(PS2DeviceType device);
};

bool initializePS2Driver();

mysty::Optional<PS2PortHandle> getPortForDevice(PS2DeviceType device);

void sendBytesToDevice(PS2PortHandle portHandle, mysty::Span<uint8_t> bytes);

} // namespace simpleos::hid

extern "C" {
__attribute__((interrupt)) void ps2Port1InterruptHandlerWrapper(void*);
ASM_CALLABLE void ps2Port1InterruptHandler();

__attribute__((interrupt)) void ps2Port2InterruptHandlerWrapper(void*);
ASM_CALLABLE void ps2Port2InterruptHandler();
}
