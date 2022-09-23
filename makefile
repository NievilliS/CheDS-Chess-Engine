# Compiler Directive
CC := gcc
CXX := g++
CRES := windres
BASH := bash

# Constants
OUTPUT_PATH := output
BUILD_DIR := build
LINUX_DIR := Linux
RELEASE_DIR := release
LIBRARIES_INCLUDE := 

# SRC List
SRCS := board.c board_file_handle.c main.c

# OBJ List
OBJS_LIN := $(SRCS:%=$(BUILD_DIR)/$(LINUX_DIR)/%.o)

# Final Step: Compile to output
default: $(BUILD_DIR)/$(LINUX_DIR) $(OBJS_LIN)
	@echo "###  DEBUG $@ BUILDING PROCEDURE  ###"
	@echo ""
	@echo "###    BUILDING LINUX DEBUG       ###"
	@echo ""
	$(CXX) $(OBJS_LIN) -o $(OUTPUT_PATH)_linux $(LIBRARIES_INCLUDE:%=-%)
	@echo ""
	@echo "###  DEBUG $@ FINISHED BUILDING   ###"

# Makedir Linux dir
$(BUILD_DIR)/$(LINUX_DIR): $(BUILD_DIR)
	mkdir $(BUILD_DIR)/$(LINUX_DIR)

# Makedir Build Dir
$(BUILD_DIR):
	mkdir $(BUILD_DIR)

# Generic C compilation linux
$(BUILD_DIR)/$(LINUX_DIR)/%.c.o: %.c
	@echo "###    BUILD $@ from $< @LINUX    ###"
	$(CC) -o $@ -c $<
	@echo "###         C@LINUX BUILT         ###"
	@echo ""

# Generic CPP compilation linux
$(BUILD_DIR)/$(LINUX_DIR)/%.cpp.o: %.cpp
	@echo "###    BUILD $@ from $< @LINUX    ###"
	$(CXX) -o $@ -c $<
	@echo "###        C++@LINUX BUILT        ###"
	@echo ""

# Build Win32 and Linux release
$(MAKECMDGOALS): $(BUILD_DIR)/$(LINUX_DIR) $(OBJS_LIN)
	@echo "### RELEASE $@ BUILDING PROCEDURE ###"
	@echo "###       MAKE RELEASE DIR        ###"
	@echo ""
	-mkdir $(RELEASE_DIR)/$@
	@echo ""
	@echo "###    BUILDING LINUX RELEASE     ###"
	@echo ""
	$(CXX) $(OBJS_LIN) -o $(RELEASE_DIR)/$@/$@_linux $(LIBRARIES_INCLUDE:%=-%)
	@echo ""
	@echo "### RELEASE $@ FINISHED BUILDING  ###"

# Makedir Release dir
$(RELEASE_DIR):
	mkdir $(RELEASE_DIR)

# Clear bin folder
.PHONY: clean
clean:
	@echo "###    CLEAN BUILD DIRECTORIES    ###"
	@echo ""
	rm -r build
	rm $(OUTPUT_PATH)_linux
	@echo ""
	@echo "###              DONE             ###"