BUILD_DIR=build

bootable_floppy: build/main.img

build/main.img: build/bootloader/boot/main.bin build/kernel/main.bin res/test.txt build/bootloader/stage2/main.bin
	"./tools/fat_imgen" -c -F -f build/main.img -s build/bootloader/boot/main.bin
	"./tools/fat_imgen" -m -f build/main.img -i res/test.txt -n test.txt
	"./tools/fat_imgen" -m -f build/main.img -i build/bootloader/stage2/main.bin -n stage2.bin
	"./tools/fat_imgen" -m -f build/main.img -i build/kernel/main.bin -n kernel.bin

build/bootloader/boot/main.bin: src/bootloader/boot/main.asm
	@mkdir -p build
	@mkdir -p build/bootloader
	@mkdir -p build/bootloader/boot
	nasm src/bootloader/boot/main.asm -f bin -o build/bootloader/boot/main.bin

build/bootloader/stage2/main.bin: src/bootloader/stage2/main.asm
	@mkdir -p build
	@mkdir -p build/bootloader
	@mkdir -p build/bootloader/stage2
	nasm src/bootloader/stage2/main.asm -f bin -o build/bootloader/stage2/main.bin

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
TOOLS_DIR=tools
include utils/omflink/Makefile

clean:
	rm build -f -R
	rm tools/omflink.exe -f
