rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))
STAGE3__RAW_SRCS:=$(call rwildcard,$(SRC_DIR),*.cpp)
STAGE3__SRCS:=$(STAGE3__RAW_SRCS:$(SRC_DIR)/%=%)
STAGE3__OBJS:=$(STAGE3__SRCS:.cpp=.obj)

STAGE3__ASM_RAW_SRCS:=$(filter-out $(SRC_DIR)/protected_setup.asm,$(call rwildcard,$(SRC_DIR),*.asm))
STAGE3__ASM_SRCS:=$(STAGE3__ASM_RAW_SRCS:$(SRC_DIR)/%=%)
STAGE3__ASM_OBJS:=$(STAGE3__ASM_SRCS:.asm=.obj)

STAGE3__BUILD_DIR:=$(BUILD_DIR)
STAGE3__SRC_DIR:=$(SRC_DIR)

all: stage3

stage3: $(STAGE3__BUILD_DIR)/main.bin

$(STAGE3__BUILD_DIR)/%.obj: $(STAGE3__SRC_DIR)/%.asm
	@mkdir -p $(STAGE3__BUILD_DIR)
	@echo "Assembling $<"
	@nasm $< -f elf64 -o $@

$(STAGE3__BUILD_DIR)/protected_setup.bin: $(STAGE3__SRC_DIR)/protected_setup.asm
	@mkdir -p $(STAGE3__BUILD_DIR)
	@echo "Assembling $<"
	@nasm $< -f bin -o $@

$(STAGE3__BUILD_DIR)/%.obj:  $(STAGE3__SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	@echo "Compiling $<"
	@$(TARGET_CXX) $(TARGET_CFLAGS) -I$(STAGE3__SRC_DIR) -MMD -o $@ $<

$(STAGE3__BUILD_DIR)/main.bin: $(addprefix $(STAGE3__BUILD_DIR)/,$(STAGE3__OBJS)) $(addprefix $(STAGE3__BUILD_DIR)/,$(STAGE3__ASM_OBJS)) $(STAGE3__BUILD_DIR)/protected_setup.bin $(STAGE3__SRC_DIR)/linker.ld
	@echo "Linking Stage3"
	@$(TARGET_LD) $(TARGET_LINKFLAGS) -o $(STAGE3__BUILD_DIR)/main.bin -Xlinker -Map $(STAGE3__BUILD_DIR)/map.txt -T $(STAGE3__SRC_DIR)/linker.ld $(addprefix $(STAGE3__BUILD_DIR)/,$(STAGE3__OBJS)) $(addprefix $(STAGE3__BUILD_DIR)/,$(STAGE3__ASM_OBJS))

# $(STAGE3__BUILD_DIR)/main.bin: $(STAGE3__BUILD_DIR)/main.bin
# 	objcopy -O binary $(STAGE3__BUILD_DIR)/main.bin $(STAGE3__BUILD_DIR)/main.bin

-include $(addprefix $(STAGE3__BUILD_DIR)/,$(STAGE3__OBJS:.obj=.d))
