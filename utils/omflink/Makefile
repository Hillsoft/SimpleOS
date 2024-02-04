OMFLINK__BUILD_DIR=$(BUILD_DIR)
OMFLINK__SRC_DIR=$(SRC_DIR)


OMFLINK__SRCS=fixup.cpp layout.cpp linkscript.cpp main.cpp nameset.cpp objloader.cpp omfrecord.cpp omfunit.cpp
OMFLINK__OBJS=$(OMFLINK__SRCS:.cpp=.o)

all: omflink

omflink: $(TOOLS_DIR)/omflink

$(TOOLS_DIR)/omflink: $(OMFLINK__BUILD_DIR)/omflink
	@mkdir -p $(TOOLS_DIR)
	cp $(OMFLINK__BUILD_DIR)/omflink $(TOOLS_DIR)/omflink

$(OMFLINK__BUILD_DIR)/omflink: $(addprefix $(OMFLINK__BUILD_DIR)/,$(OMFLINK__OBJS))
	+$(CXX) $(LDFLAGS) -o $(OMFLINK__BUILD_DIR)/omflink $(addprefix $(OMFLINK__BUILD_DIR)/,$(OMFLINK__OBJS))

$(OMFLINK__BUILD_DIR)/%.o: $(OMFLINK__SRC_DIR)/%.cpp
	@mkdir -p $(OMFLINK__BUILD_DIR)
	$(CXX) $(CPPFLAGS) -MMD -o $@ $< -c $(CFLAGS)

-include $(addprefix $(OMFLINK__BUILD_DIR)/,$(OMFLINK__OBJS:.o=.d))
