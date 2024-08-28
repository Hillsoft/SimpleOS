all: fat_demo omflink bootable_floppy

include build_scripts/config.mk
include build_scripts/toolchain.mk

CXX_WARNING_FLAGS=-pedantic -Wall -Wextra -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 -Winit-self -Wlogical-op -Wmissing-declarations -Wmissing-include-dirs -Wnoexcept -Wold-style-cast -Woverloaded-virtual -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=5 -Wswitch-default -Wundef -Werror -Wno-unused

CPPFLAGS_RELEASE=--std=c++20 -O3 -flto -march=native
LDFLAGS_RELEASE=-s -flto=auto -O3

CPPFLAGS_DEBUG=--std=c++20 -O2 -g -march=native
LDFLAGS_DEBUG=

CPPFLAGS=$(CPPFLAGS_RELEASE) $(CXX_WARNING_FLAGS)
LDFLAGS=$(LDFLAGS_RELEASE)

TOOLS_DIR=tools


bootable_floppy: build/main.img

build/main.img: build/bootloader/boot/main.bin build/kernel/main.bin res/test.txt build/bootloader/stage3/main.bin
	dd if=/dev/zero of=$@ bs=512 count=2880 >/dev/null
	mkfs.fat -F 12 -n "NBOS" $@ >/dev/null
	dd if=build/bootloader/boot/main.bin of=$@ conv=notrunc >/dev/null
	mcopy -i $@ build/bootloader/stage3/main.bin "::stage3.bin"
	mcopy -i $@ res/test.txt "::test.txt"
	mcopy -i $@ build/kernel/main.bin "::kernel.bin"

build/bootloader/boot/main.bin: src/bootloader/boot/main.asm
	@mkdir -p build
	@mkdir -p build/bootloader
	@mkdir -p build/bootloader/boot
	nasm src/bootloader/boot/main.asm -f bin -o build/bootloader/boot/main.bin

BUILD_DIR=build/bootloader/stage3
SRC_DIR=src/bootloader/stage3
include src/bootloader/stage3/Makefile

build/kernel/main.bin: src/kernel/main.asm
	@mkdir -p build
	@mkdir -p build/kernel
	nasm src/kernel/main.asm -f bin -o build/kernel/main.bin

fat_demo: build/utils/fat.exe

build/utils/fat.exe: utils/fat/fat.cpp
	@mkdir -p build/utils
	g++ -g utils/fat/fat.cpp -o build/utils/fat.exe -std=c++20 -static -static-libgcc -static-libstdc++

BUILD_DIR=build/omflink
SRC_DIR=utils/omflink
include utils/omflink/Makefile

clean:
	rm build -f -R
	rm tools/omflink.exe -f
