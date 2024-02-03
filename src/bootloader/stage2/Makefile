STAGE2__SRCS=disk.asm fat.asm main.asm memory.asm printf.asm printutils.asm protected.asm string.asm
STAGE2__OBJS=$(STAGE2__SRCS:.asm=.o)

STAGE2__BUILD_DIR:=$(BUILD_DIR)
STAGE2__SRC_DIR:=$(SRC_DIR)

all: stage2

stage2: $(STAGE2__BUILD_DIR)/main.bin

$(STAGE2__BUILD_DIR)/main.bin: $(TOOLS_DIR)/omflink.exe $(STAGE2__SRC_DIR)/linker.ld $(addprefix $(STAGE2__BUILD_DIR)/,$(STAGE2__OBJS))
	$(TOOLS_DIR)/omflink.exe $(STAGE2__SRC_DIR)/linker.ld $(STAGE2__BUILD_DIR)/main.bin $(addprefix $(STAGE2__BUILD_DIR)/,$(STAGE2__OBJS))

$(STAGE2__BUILD_DIR)/%.o: $(STAGE2__SRC_DIR)/%.asm
	@mkdir -p $(STAGE2__BUILD_DIR)
	nasm $< -f obj -o $@
