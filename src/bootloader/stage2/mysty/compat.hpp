#pragma once

#define ASM_CALLABLE \
  __attribute__((sysv_abi)) __attribute__((externally_visible))
