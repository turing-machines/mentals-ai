TARGET_EXEC := mentals

BUILD_DIR := ./build
SRC_DIRS := ./src

LOGS_DIR := ./logs

VERBOSE ?= 0

# Find all the C and C++ files we want to compile
SRCS := $(shell find $(SRC_DIRS) -name '*.cpp' -or -name '*.c' -or -name '*.s')

# Prepends BUILD_DIR and appends .o to every src file
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)

# String substitution for dependency files
DEPS := $(OBJS:.o=.d)

# Every folder in ./src will need to be passed to the compiler so that it can find header files
INC_DIRS := $(shell find $(SRC_DIRS) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

# Add Poppler include directory
POPPLER_INCLUDE := $(shell pkg-config --cflags-only-I poppler-cpp)
INC_FLAGS += $(POPPLER_INCLUDE)

# Conditional configuration for OS-specific flags
OS := $(shell uname -s)
ifeq ($(OS),Linux)
    CPPFLAGS += -DLINUX
    LDFLAGS := -lrt -lpthread -lcurl -lfmt -lpqxx -lpq $(shell pkg-config --libs poppler-cpp)
endif
ifeq ($(OS),Darwin)
    CPPFLAGS += -DMACOS
    FMT_INCLUDE := $(shell pkg-config --cflags fmt)
    INC_FLAGS += $(FMT_INCLUDE)
    PQXX_INCLUDE := $(shell pkg-config --cflags libpqxx)
    INC_FLAGS += $(PQXX_INCLUDE)
    
    # explicit pqxx paths
    PG_CONFIG := /opt/homebrew/opt/postgresql@14/bin/pg_config
    PG_INCLUDE := $(shell $(PG_CONFIG) --includedir)
    PG_LIB := $(shell $(PG_CONFIG) --libdir)
    INC_FLAGS += -I$(PG_INCLUDE)
    
    LDFLAGS := -L$(PG_LIB) \
               -L/opt/homebrew/lib \
               -lpq -lpqxx -lfmt \
               -lpthread -lcurl \
               -L/opt/homebrew/Cellar/poppler/25.01.0/lib -lpoppler-cpp \
               -Wl,-rpath,$(PG_LIB)
endif
ifeq ($(OS),Windows_NT)
    CPPFLAGS += -DWIN32
    LDFLAGS := -lws2_32 -lcurl -lfmt -lpqxx -lpq
endif

ifeq ($(VERBOSE),1)
    CXXFLAGS += -v
    LDFLAGS += -v
endif

# The -MMD and -MP flags together generate Makefiles for us!
CPPFLAGS += $(INC_FLAGS) -MMD -MP #-D__PGVECTOR__
CXXFLAGS := -std=c++20 -Wall -Wextra -O3 -march=native # -Werror

# Default to clang if available and version is >= 14, otherwise use gcc
CXX := $(shell which clang++ || which g++)
ifeq ($(shell $(CXX) --version | grep -q "clang" && echo "yes"),yes)
    CXX_VERSION := $(shell $(CXX) --version | sed -n 's/.*version \([0-9]*\).*/\1/p' | head -n 1)
else
    CXX_VERSION := $(shell $(CXX) --version | sed -n 's/^.* \([0-9]*\)\..*/\1/p' | head -n 1)
endif
ifeq ($(CXX),$(shell which clang++))
    ifneq ($(shell expr $(CXX_VERSION) \>= 14),1)
        CXX := g++
    endif
else
    CXX := g++
endif

CC := $(CXX)

# Output chosen compiler and version
$(info $(shell echo "\033[0;33mUsing compiler: $(CXX) version $(CXX_VERSION)\033[0m"))

# The final build step.
$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)

# Build step for C++ source
$(BUILD_DIR)/%.cpp.o: %.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

.PHONY: clean cleanlogs
clean:
	rm -rf $(BUILD_DIR)

cleanlogs:
	rm -rf $(LOGS_DIR)/*

# Include the .d makefiles. The - at the front suppresses errors of missing
# Makefiles. Initially, all the .d files will be missing, and we don't want those
# errors to show up.
-include $(DEPS)
