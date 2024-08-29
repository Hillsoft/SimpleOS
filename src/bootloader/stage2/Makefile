rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))
STAGE2__RAW_SRCS:=$(call rwildcard,$(SRC_DIR),*.cpp)
STAGE2__SRCS:=$(STAGE2__RAW_SRCS:$(SRC_DIR)/%=%)
STAGE2__OBJS:=$(STAGE2__SRCS:.cpp=.obj)

STAGE2__ASM_RAW_SRCS:=$(filter-out $(SRC_DIR)/protected_setup.asm,$(call rwildcard,$(SRC_DIR),*.asm))
STAGE2__ASM_SRCS:=$(STAGE2__ASM_RAW_SRCS:$(SRC_DIR)/%=%)
STAGE2__ASM_OBJS:=$(STAGE2__ASM_SRCS:.asm=.obj)

STAGE2__BUILD_DIR:=$(BUILD_DIR)
STAGE2__SRC_DIR:=$(SRC_DIR)

all: stage2

stage2: $(STAGE2__BUILD_DIR)/main.bin

$(STAGE2__BUILD_DIR)/%.obj: $(STAGE2__SRC_DIR)/%.asm
	@mkdir -p $(STAGE2__BUILD_DIR)
	@echo "Assembling $<"
	@nasm $< -f elf64 -o $@

$(STAGE2__BUILD_DIR)/protected_setup.bin: $(STAGE2__SRC_DIR)/protected_setup.asm
	@mkdir -p $(STAGE2__BUILD_DIR)
	@echo "Assembling $<"
	@nasm $< -f bin -o $@

$(STAGE2__BUILD_DIR)/%.obj:  $(STAGE2__SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	@echo "Compiling $<"
	@$(TARGET_CXX) $(TARGET_CFLAGS) -I$(STAGE2__SRC_DIR) -I$(MYSTY__SRC_DIR) -MMD -o $@ $<

$(STAGE2__BUILD_DIR)/main.bin: $(addprefix $(STAGE2__BUILD_DIR)/,$(STAGE2__OBJS)) $(addprefix $(STAGE2__BUILD_DIR)/,$(STAGE2__ASM_OBJS)) $(STAGE2__BUILD_DIR)/protected_setup.bin $(STAGE2__SRC_DIR)/linker.ld $(addprefix $(MYSTY__BUILD_DIR)/,$(MYSTY__OBJS))
	@echo "Linking Stage2"
	@$(TARGET_LD) $(TARGET_LINKFLAGS) -o $(STAGE2__BUILD_DIR)/main.bin -Xlinker -Map $(STAGE2__BUILD_DIR)/map.txt -T $(STAGE2__SRC_DIR)/linker.ld $(addprefix $(STAGE2__BUILD_DIR)/,$(STAGE2__OBJS)) $(addprefix $(MYSTY__BUILD_DIR)/,$(MYSTY__OBJS)) $(addprefix $(STAGE2__BUILD_DIR)/,$(STAGE2__ASM_OBJS))

# $(STAGE2__BUILD_DIR)/main.bin: $(STAGE2__BUILD_DIR)/main.bin
# 	objcopy -O binary $(STAGE2__BUILD_DIR)/main.bin $(STAGE2__BUILD_DIR)/main.bin

-include $(addprefix $(STAGE2__BUILD_DIR)/,$(STAGE2__OBJS:.obj=.d))
