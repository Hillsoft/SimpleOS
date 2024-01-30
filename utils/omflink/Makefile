BUILD_DIR?=build
TOOLS_DIR?=tools

SRC_DIR?=.

CXX=g++
CPPFLAGS=--std=c++20 -O2 -s -march=native $(CXX_WARNING_FLAGS)

LDFLAGS=

SRCS=fixup.cpp layout.cpp main.cpp nameset.cpp objloader.cpp omfrecord.cpp omfunit.cpp
OBJS=$(SRCS:.cpp=.o)

all: omflink

omflink: $(TOOLS_DIR)/omflink.exe

$(TOOLS_DIR)/omflink.exe: $(BUILD_DIR)/omflink.exe
	@mkdir -p $(TOOLS_DIR)
	cp $(BUILD_DIR)/omflink.exe $(TOOLS_DIR)/omflink.exe

$(BUILD_DIR)/omflink.exe: $(addprefix $(BUILD_DIR)/,$(OBJS))
	$(CXX) $(LDFLAGS) -o $(BUILD_DIR)/omflink.exe $(addprefix $(BUILD_DIR)/,$(OBJS))

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CPPFLAGS) -MMD -o $@ $< -c $(CFLAGS)

-include $(addprefix $(BUILD_DIR)/,$(OBJS:.o=.d))
