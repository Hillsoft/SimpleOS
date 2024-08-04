export CFLAGS = 
export ASMFLAGS =
export CC = gcc
export CXX = g++-12
export LD = gcc
export ASM = nasm
export LINKFLAGS =
export LIBS =

export TARGET = i686-elf
export TARGET_ASM = nasm
export TARGET_ASMFLAGS =
export TARGET_CFLAGS = -std=c++20 -O3 -flto -c -ffreestanding -fno-exceptions -fno-asynchronous-unwind-tables -fno-rtti -nostdinc -nostdlib -pedantic -Wall -Wextra -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 -Winit-self -Wlogical-op -Wmissing-declarations -Wmissing-include-dirs -Wnoexcept -Wold-style-cast -Woverloaded-virtual -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=0 -Wswitch-default -Wundef -Werror -Wno-unused
export TARGET_CC = $(TARGET)-gcc
export TARGET_CXX = $(TARGET)-g++
export TARGET_LD = $(TARGET)-g++
export TARGET_LINKFLAGS = -flto=auto -fwhole-program -O3 -nostdinc -nostdlib
export TARGET_LIBS =

export BUILD_DIR = $(abspath build)

BINUTILS_VERSION = 2.42
BINUTILS_URL = https://ftp.gnu.org/gnu/binutils/binutils-$(BINUTILS_VERSION).tar.xz

GCC_VERSION = 13.2.0
GCC_URL = https://ftp.gnu.org/gnu/gcc/gcc-$(GCC_VERSION)/gcc-$(GCC_VERSION).tar.xz
