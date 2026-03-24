SHELL        := /bin/zsh

CXX          := g++
CXXFLAGS     := -std=c++23 -Wall -Wextra -g -O3 -fdiagnostics-color=always -MMD -MP

LIB_CFLAGS   := -I/usr/include/SDL3
LIBS_LDFLAGS := -lSDL3

BUILD_DIR    := build
TARGET       := main

SOURCES      := $(wildcard src/*.cpp src/**/*.cpp)
OBJECTS      := $(patsubst src/%.cpp, $(BUILD_DIR)/%.o, $(SOURCES))
DEPS         := $(OBJECTS:.o=.d)
OBJ_DIRS     := $(sort $(dir $(OBJECTS)))

-include $(DEPS)

ifeq ($(F),)
  TARGET  := main
  OBJECTS := $(OBJECTS)
else
  TARGET  := $(F)
  OBJECTS := $(BUILD_DIR)/$(F).o
  SRC     := $(wildcard src/$(F).cpp)
  $(BUILD_DIR)/$(F).o: $(SRC) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(LIB_CFLAGS) -Iinclude -c $< -o $@
endif

all: compile

run: compile
	./$(BUILD_DIR)/$(TARGET)

compile: $(BUILD_DIR)/$(TARGET)

$(BUILD_DIR)/$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $@ $(LIBS_LDFLAGS)

$(BUILD_DIR)/%.o: src/%.cpp | $(OBJ_DIRS)
	$(CXX) $(CXXFLAGS) $(LIB_CFLAGS) -Iinclude -c $< -o $@

$(OBJ_DIRS):
	mkdir -p $@

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all compile run clean
