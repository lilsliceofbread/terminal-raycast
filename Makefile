NAME := raycast
CXX := g++
CXX_FLAGS := -g -Wall -I./inc -std=c++17
# CPP_FLAGS := -MMD -MP # pre-processor flags

BUILD_DIR := ./build
OBJ_DIR := ./build/obj

SOURCES := $(wildcard ./src/*.cpp)
OBJS := $(patsubst ./src/%.cpp, $(OBJ_DIR)/%.o, $(SOURCES)) # subsitute .cpp files into obj path with .o
# DEPS := $(OBJS:.o=.d) # substitute .o for .d in all objs

.PHONY: directories
all: directories $(NAME)

directories:
	mkdir -p $(BUILD_DIR)
	mkdir -p $(OBJ_DIR)

$(NAME): $(OBJS)	
	$(CXX) $(CXX_FLAGS) $(OBJS) -o $(BUILD_DIR)/$(NAME) -lX11

$(OBJ_DIR)/%.o: ./src/%.cpp # can't use $(SOURCE) or $(OBJS) because weird pattern rules
	$(CXX) $(CXX_FLAGS) -c $^ -o $@