# Compiler Directive
CC := gcc
CXX := g++
CRES := windres

# Constants
OUTPUT_PATH := o.exe
BUILD_DIR := build
LIBRARIES_INCLUDE := 

# SRC List
SRCS := board.c main.c

# OBJ List
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)

# Final Step: Compile to output
$(OUTPUT_PATH): $(BUILD_DIR) $(OBJS)
	$(CXX) $(OBJS) -o $(OUTPUT_PATH) $(LIBRARIES_INCLUDE:%=-%)

# Makedir Build Dir
$(BUILD_DIR):
	mkdir $(BUILD_DIR)

# Generic C compilation
$(BUILD_DIR)/%.c.o: %.c
	$(CC) -o $@ -c $<

# Generic C++ compilation
$(BUILD_DIR)\%.cpp.o: %.cpp
	$(CXX) -o $@ -c $<

# Generic WINRES compilation
$(BUILD_DIR)\%.rc.o: %.rc
	$(CRES) $< -o $@

# Clear bin folder
.PHONY: clean
clean:
	del /q /f $(BUILD_DIR)\*