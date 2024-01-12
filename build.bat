mkdir build
mkdir "build/kernel"
mkdir "build/bootloader"

nasm src/bootloader/main.asm -f bin -o build/bootloader/main.bin
nasm src/kernel/main.asm -f bin -o build/kernel/main.bin

"./tools/fat_imgen" -c -F -f build/main.img -s build/bootloader/main.bin -i res/test.txt -n test.txt