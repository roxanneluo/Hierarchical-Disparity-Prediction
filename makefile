CXX := g++-4.8

BUILD_DIR := bin
SRC_DIRS := main checker util gen_data
INCLUDE_DIRS := include

#main
MAIN_SRCS := $(shell find main -name "*.cpp")
MAIN_BINS := $(addprefix $(BUILD_DIR)/, ${MAIN_SRCS:.cpp=.bin})
#checker
CHECKER_SRCS := $(shell find checker -name "*.cpp")
CHECKER_BINS := $(addprefix $(BUILD_DIR)/, ${CHECKER_SRCS:.cpp=.bin})
#util
UTIL_SRCS := $(shell find util -name "*.cpp")
UTIL_BINS := $(addprefix $(BUILD_DIR)/, ${UTIL_SRCS:.cpp=.bin})
#gen_data
GEN_DATA_SRCS := $(shell find gen_data -name "*.cpp")
GEN_DATA_BINS := $(addprefix $(BUILD_DIR)/, ${GEN_DATA_SRCS:.cpp=.bin})

ALL_BINS := $(MAIN_BINS) $(CHECKER_BINS) $(UTIL_BINS) $(GEN_DATA_BINS)

ALL_BUILD_DIRS := $(addprefix $(BUILD_DIR)/, $(SRC_DIRS))
CXX_FLAGS := -O2 -msse3 -std=c++0x $(foreach dir, $(INCLUDE_DIRS), -I$(dir))

all: main checker util gen_data
init:$(ALL_BUILD_DIRS)
main: init echo $(MAIN_BINS)
checker: init $(CHECKER_BINS)
util: init $(UTIL_BINS)
gen_data: init $(GEN_DATA_BINS)

$(ALL_BUILD_DIRS):
	mkdir -p $@
$(BUILD_DIR)/%.bin: %.cpp
	$(CXX) $< -o $@ $(CXX_FLAGS)
clean:
	rm -rf $(BUILD_DIR)

echo:
	echo $(MAIN_BINS)
