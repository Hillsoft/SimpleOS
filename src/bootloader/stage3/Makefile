STAGE3__SRCS=after_protected.asm
STAGE3__OBJS=$(STAGE3__SRCS:.asm=.o)

STAGE3__BUILD_DIR:=$(BUILD_DIR)
STAGE3__SRC_DIR:=$(SRC_DIR)

all: stage3

stage3: $(STAGE3__BUILD_DIR)/main.bin

$(STAGE3__BUILD_DIR)/main.bin: $(STAGE3__SRC_DIR)/after_protected.asm
	@mkdir -p $(STAGE3__BUILD_DIR)
	nasm $< -f bin -o $@

