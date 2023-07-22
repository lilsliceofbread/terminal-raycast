NAME := raycast
CXX := g++
CXX_FLAGS := -g -Wall -Iinc -std=c++17

ifeq ($(OS),Windows_NT)
	LINK_FLAGS := -lpdcurses
else
	ifeq ($(shell uname -s),Linux)
		LINK_FLAGS := -lncurses
	endif
endif
# CPP_FLAGS := -MMD -MP # pre-processor flags

BUILD_DIR := build
OBJ_DIR := build/obj

SOURCES := $(wildcard ./src/*.cpp)
# subsitute .cpp files into obj path with .o
OBJS := $(patsubst ./src/%.cpp, $(OBJ_DIR)/%.o, $(SOURCES))
# DEPS := $(OBJS:.o=.d) # substitute .o for .d in all objs

.PHONY: directories
all: directories $(NAME)

directories:
	mkdir -p $(BUILD_DIR)
	mkdir -p $(OBJ_DIR)

$(NAME): $(OBJS)	
	$(CXX) $(CXX_FLAGS) $(OBJS) $(LINK_FLAGS) -o $(BUILD_DIR)/$(NAME)

$(OBJ_DIR)/%.o: ./src/%.cpp # can't use $(SOURCE) or $(OBJS) because weird pattern rules
	$(CXX) $(CXX_FLAGS) -c $^ -o $@