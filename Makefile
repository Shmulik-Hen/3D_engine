PRJ_NAME   := 3D_engine
SRC_DIR    := ./src
INC_DIR    := ./inc
BLD_DIR    := ./bin
CFG_DIR	   := ./config
VAL_DIR    := ./validations
TARGET     := $(BLD_DIR)/$(PRJ_NAME)
SRCS       := $(sort $(shell find $(SRC_DIR) -name '*.cpp' | tr '\n' ' '))
OBJS       := $(subst $(SRC_DIR),$(BLD_DIR),$(SRCS:%.cpp=%.o))
DEPS       := $(OBJS:.o=.d)
INC_FLAGS  := $(addprefix -I,$(INC_DIR))
CPPFLAGS   := -MMD -MP
#LDFLAGS   :=
CXXFLAGS   += -std=c++17 -Wall -Wextra -Wpedantic $(INC_FLAGS)

SDL_FLAGS := $(shell sdl2-config --cflags)
SDL_LIBS  := $(shell sdl2-config --libs)
CXXFLAGS  += $(SDL_FLAGS)
LDLIBS    += $(SDL_LIBS)

JSON_FLAGS := $(shell pkg-config --cflags jsoncpp)
JSON_LIBS  := $(shell pkg-config --libs jsoncpp)
CXXFLAGS   += $(JSON_FLAGS)
LDLIBS     += $(JSON_LIBS)

#DEBUG := 1
ifeq ($(DEBUG), 1)
CXXFLAGS += -g -O0
else
CXXFLAGS += -O2
endif

ifeq ($(DBG_ASAN), 1)
CXXFLAGS += -fsanitize=address,undefined
LDFLAGS += -fsanitize=address,undefined
endif

ifeq ($(DBG_PRINTS), 1)
CXXFLAGS += $(addprefix -D,DEBUG_PRINTS)
endif

ifeq ($(DBG_POLY), 1)
CXXFLAGS += $(addprefix -D,DEBUG_POLYGON)
endif

.PHONY: all

all: build

$(BLD_DIR):
	mkdir -p $(BLD_DIR)

$(BLD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BLD_DIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

$(TARGET): $(OBJS)
	$(CXX) $^ -o $@ $(LDFLAGS) $(LDLIBS)

build: $(TARGET)

rebuild: clean build

clean:
	rm -rf $(BLD_DIR) $(VAL_DIR)

print:
	@echo "ARCH='$(ARCH)'"
	@echo "CROSS_COMPILE='$(CROSS_COMPILE)'"
	@echo "TARGET='$(TARGET)'"
	@echo "SRCS='$(SRCS)'"
	@echo "OBJS='$(OBJS)'"
	@echo "DEPS='$(DEPS)'"
	@echo "CXX='$(CXX)'"
	@echo "FLAGS='$(CPPFLAGS)' '$(CXXFLAGS)' '$(LDFLAGS)' '$(LDLIBS)'"

-include $(DEPS)
