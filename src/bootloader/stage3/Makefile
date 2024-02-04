STAGE3__SRCS=after_protected.asm protected_setup.asm
STAGE3__OBJS=$(STAGE3__SRCS:.asm=.obj)

STAGE3__BUILD_DIR:=$(BUILD_DIR)
STAGE3__SRC_DIR:=$(SRC_DIR)

all: stage3

stage3: $(STAGE3__BUILD_DIR)/main.bin

$(STAGE3__BUILD_DIR)/after_protected.obj: $(STAGE3__SRC_DIR)/after_protected.asm
	@mkdir -p $(STAGE3__BUILD_DIR)
	nasm $< -f elf -o $@

$(STAGE3__BUILD_DIR)/protected_setup.bin: $(STAGE3__SRC_DIR)/protected_setup.asm
	@mkdir -p $(STAGE3__BUILD_DIR)
	nasm $< -f bin -o $@

$(STAGE3__BUILD_DIR)/%.obj:  $(STAGE3__SRC_DIR)/%.cpp $(STAGE3__SRC_DIR)/linker.ld
	@mkdir -p $(STAGE3__BUILD_DIR)
	$(TARGET_CXX) -c -ffreestanding -fno-exceptions -fno-asynchronous-unwind-tables -nostdinc -nostdlib -o $(STAGE3__BUILD_DIR)/main.obj $(STAGE3__SRC_DIR)/main.cpp

$(STAGE3__BUILD_DIR)/main.bin:  $(STAGE3__BUILD_DIR)/main.obj $(STAGE3__BUILD_DIR)/after_protected.obj $(STAGE3__BUILD_DIR)/protected_setup.bin $(STAGE3__SRC_DIR)/linker.ld
	$(TARGET_LD) -o $(STAGE3__BUILD_DIR)/main.bin --disable-reloc-section --disable-runtime-pseudo-reloc --disable-large-address-aware -nostdinc -nostdlib -T $(STAGE3__SRC_DIR)/linker.ld $(STAGE3__BUILD_DIR)/main.obj $(STAGE3__BUILD_DIR)/after_protected.obj

# $(STAGE3__BUILD_DIR)/main.bin: $(STAGE3__BUILD_DIR)/main.bin
# 	objcopy -O binary $(STAGE3__BUILD_DIR)/main.bin $(STAGE3__BUILD_DIR)/main.bin
