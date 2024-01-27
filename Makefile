bootable_floppy: build/main.img

build/main.img: build/bootloader/main.bin build/kernel/main.bin res/test.txt build/bootloader/stage2.bin
	"./tools/fat_imgen" -c -F -f build/main.img -s build/bootloader/main.bin
	"./tools/fat_imgen" -m -f build/main.img -i res/test.txt -n test.txt
	"./tools/fat_imgen" -m -f build/main.img -i build/bootloader/stage2.bin -n stage2.bin
	"./tools/fat_imgen" -m -f build/main.img -i build/kernel/main.bin -n kernel.bin

build/bootloader/main.bin: src/bootloader/main.asm
	@mkdir -p build
	@mkdir -p build/bootloader
	nasm src/bootloader/main.asm -f bin -o build/bootloader/main.bin

build/bootloader/stage2.bin: src/bootloader/stage2.asm
	@mkdir -p build
	@mkdir -p build/bootloader
	nasm src/bootloader/stage2.asm -f bin -o build/bootloader/stage2.bin

build/kernel/main.bin: src/kernel/main.asm
	@mkdir -p build
	@mkdir -p build/kernel
	nasm src/kernel/main.asm -f bin -o build/kernel/main.bin

fat_demo: build/utils/fat.exe

build/utils/fat.exe: utils/fat/fat.cpp
	@mkdir -p build/utils
	g++ -g utils/fat/fat.cpp -o build/utils/fat.exe -std=c++20 -static -static-libgcc -static-libstdc++

clean:
	rm build -f -R
