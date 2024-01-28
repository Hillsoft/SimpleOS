BUILD_DIR?=build

CXX=g++
CPPFLAGS=--std=c++20

SRCS=main.cpp omfrecord.cpp
OBJS=$(SRCS:.cpp=.o)

all: omflink

omflink: $(BUILD_DIR)/omflink.exe

$(BUILD_DIR)/omflink.exe: $(addprefix $(BUILD_DIR)/,$(OBJS))
	$(CXX) $(LDFLAGS) -o $(BUILD_DIR)/omflink.exe $(addprefix $(BUILD_DIR)/,$(OBJS))

$(BUILD_DIR)/%.o: %.cpp
	$(CXX) $(CPPFLAGS) -MMD -o $@ $< -c $(CFLAGS)

clean:
	rm -f $(BUILD_DIR)

-include $(BUILD_DIR)/$(OBJS:.o=.d)
