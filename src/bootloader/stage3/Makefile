rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))
STAGE3__RAW_SRCS:=$(call rwildcard,$(SRC_DIR),*.cpp)
STAGE3__SRCS:=$(STAGE3__RAW_SRCS:$(SRC_DIR)/%=%)
STAGE3__OBJS:=$(STAGE3__SRCS:.cpp=.obj)

STAGE3__BUILD_DIR:=$(BUILD_DIR)
STAGE3__SRC_DIR:=$(SRC_DIR)

all: stage3

stage3: $(STAGE3__BUILD_DIR)/main.bin

$(STAGE3__BUILD_DIR)/after_protected.obj: $(STAGE3__SRC_DIR)/after_protected.asm
	@mkdir -p $(STAGE3__BUILD_DIR)
	nasm $< -f elf -o $@

$(STAGE3__BUILD_DIR)/x86.obj: $(STAGE3__SRC_DIR)/x86.asm
	@mkdir -p $(STAGE3__BUILD_DIR)
	nasm $< -f elf -o $@

$(STAGE3__BUILD_DIR)/protected_setup.bin: $(STAGE3__SRC_DIR)/protected_setup.asm
	@mkdir -p $(STAGE3__BUILD_DIR)
	nasm $< -f bin -o $@

$(STAGE3__BUILD_DIR)/%.obj:  $(STAGE3__SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(TARGET_CXX) $(TARGET_CFLAGS) -I$(STAGE3__SRC_DIR) -MMD -o $@ $<

$(STAGE3__BUILD_DIR)/main.bin: $(addprefix $(STAGE3__BUILD_DIR)/,$(STAGE3__OBJS)) $(STAGE3__BUILD_DIR)/after_protected.obj $(STAGE3__BUILD_DIR)/x86.obj $(STAGE3__BUILD_DIR)/protected_setup.bin $(STAGE3__SRC_DIR)/linker.ld
	$(TARGET_LD) $(TARGET_LINKFLAGS) -o $(STAGE3__BUILD_DIR)/main.bin -T $(STAGE3__SRC_DIR)/linker.ld $(addprefix $(STAGE3__BUILD_DIR)/,$(STAGE3__OBJS)) $(STAGE3__BUILD_DIR)/after_protected.obj $(STAGE3__BUILD_DIR)/x86.obj

# $(STAGE3__BUILD_DIR)/main.bin: $(STAGE3__BUILD_DIR)/main.bin
# 	objcopy -O binary $(STAGE3__BUILD_DIR)/main.bin $(STAGE3__BUILD_DIR)/main.bin

-include $(addprefix $(STAGE3__BUILD_DIR)/,$(STAGE3__OBJS:.obj=.d))
