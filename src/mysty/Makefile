rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))
MYSTY__RAW_SRCS:=$(call rwildcard,$(SRC_DIR),*.cpp)
MYSTY__SRCS:=$(MYSTY__RAW_SRCS:$(SRC_DIR)/%=%)
MYSTY__OBJS:=$(MYSTY__SRCS:.cpp=.obj)

MYSTY__BUILD_DIR:=$(BUILD_DIR)
MYSTY__SRC_DIR:=$(SRC_DIR)

$(MYSTY__BUILD_DIR)/%.obj:  $(MYSTY__SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	@echo "Compiling $<"
	@$(TARGET_CXX) $(TARGET_CFLAGS) -I$(MYSTY__SRC_DIR) -MMD -o $@ $<

-include $(addprefix $(MYSTY__BUILD_DIR)/,$(MYSTY__OBJS:.obj=.d))
