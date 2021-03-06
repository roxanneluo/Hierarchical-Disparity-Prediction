CXX := g++-4.8 # g++

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
#include
INCLUDED = $(foreach dir, $(INCLUDE_DIRS), $(addprefix $(dir)/, $(shell ls $(dir))))

#ALL_BINS := $(MAIN_BINS) $(CHECKER_BINS) $(UTIL_BINS) $(GEN_DATA_BINS)
ALL_CPP := $(foreach sdir, $(SRC_DIRS), $(shell find $(sdir) -name "*.cpp"))
ALL_BINS := $(addprefix $(BUILD_DIR)/, ${ALL_CPP:.cpp=.bin})

ALL_BUILD_DIRS := $(addprefix $(BUILD_DIR)/, $(SRC_DIRS))
CXX_FLAGS := -lpng -O2 -msse3 -std=c++11 $(foreach dir, $(INCLUDE_DIRS), -I$(dir))

#all: main checker util gen_data
all: $(ALL_CPP) init $(ALL_BINS)
init:$(ALL_BUILD_DIRS)
main: init $(MAIN_BINS)
checker: init $(CHECKER_BINS)
util: init $(UTIL_BINS)
gen_data: init $(GEN_DATA_BINS)

$(ALL_BUILD_DIRS):
	mkdir -p $@

$(BUILD_DIR)/main/%.bin: main/%.cpp $(INCLUDED) 
	$(CXX) $< -o $@ $(CXX_FLAGS)

$(BUILD_DIR)/gen_data/%.bin: gen_data/%.cpp gen_data/statistics.hpp $(INCLUDED) 
	$(CXX) $< -o $@ $(CXX_FLAGS)

$(BUILD_DIR)/%.bin: %.cpp
	$(CXX) $< -o $@ $(CXX_FLAGS)

clean:
	rm -rf $(BUILD_DIR)
