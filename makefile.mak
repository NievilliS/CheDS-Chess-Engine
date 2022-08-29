# Compiler Directive
CC := gcc
CXX := g++
CRES := windres
BASH := bash

# Constants
OUTPUT_PATH := output
BUILD_DIR := build
WIN_DIR := Win32
LINUX_DIR := Linux
RELEASE_DIR := release
LIBRARIES_INCLUDE := 

# SRC List
SRCS := board.c main.c

# OBJ List
OBJS_WIN := $(SRCS:%=$(BUILD_DIR)/$(WIN_DIR)/%.o)
OBJS_LIN := $(SRCS:%=$(BUILD_DIR)/$(LINUX_DIR)/%.o)

# Final Step: Compile to output
$(OUTPUT_PATH): $(BUILD_DIR)\$(WIN_DIR) $(BUILD_DIR)\$(LINUX_DIR) $(OBJS_WIN) $(OBJS_LIN)
	@echo ###  DEBUG $@ BUILDING PROCEDURE  ###
	@echo.
	@echo ###     BUILDING WIN32 DEBUG      ###
	@echo.
	$(CXX) $(OBJS_WIN) -o $(OUTPUT_PATH)_win32.exe $(LIBRARIES_INCLUDE:%=-%)
	@echo.
	@echo ###    BUILDING LINUX RELEASE     ###
	@echo.
	$(BASH) -c "$(CXX) $(OBJS_LIN) -o $(OUTPUT_PATH)_linux.exe $(LIBRARIES_INCLUDE:%=-%)"
	@echo.
	@echo ###  DEBUG $@ FINISHED BUILDING   ###

# Makedir Win32 dir
$(BUILD_DIR)\$(WIN_DIR): $(BUILD_DIR)
	mkdir $(BUILD_DIR)\$(WIN_DIR)

# Makedir Linux dir
$(BUILD_DIR)\$(LINUX_DIR): $(BUILD_DIR)
	mkdir $(BUILD_DIR)\$(LINUX_DIR)

# Makedir Build Dir
$(BUILD_DIR):
	mkdir $(BUILD_DIR)

# Generic C compilation win32
$(BUILD_DIR)/$(WIN_DIR)/%.c.o: %.c
	@echo ###    BUILD $@ from $< @WIN32    ###
	$(CC) -o $@ -c $<
	@echo ###         C@WIN32 BUILT         ###
	@echo.

# Generic C compilation linux
$(BUILD_DIR)/$(LINUX_DIR)/%.c.o: %.c
	@echo ###    BUILD $@ from $< @LINUX    ###
	$(BASH) -c "$(CC) -o $@ -c $<"
	@echo ###         C@LINUX BUILT         ###
	@echo.

# Generic C++ compilation win32
$(BUILD_DIR)/$(WIN_DIR)/%.cpp.o: %.cpp
	@echo ###    BUILD $@ from $< @WIN32    ###
	$(CXX) -o $@ -c $<
	@echo ###        C++@WIN32 BUILT        ###
	@echo.

# Generic C compilation linux
$(BUILD_DIR)/$(LINUX_DIR)/%.cpp.o: %.cpp
	@echo ###    BUILD $@ from $< @LINUX    ###
	$(BASH) -c "$(CXX) -o $@ -c $<"
	@echo ###        C++@LINUX BUILT        ###
	@echo.

# Generic WINRES compilation
$(BUILD_DIR)/%.rc.o: %.rc
	@echo ###    BUILD $@ from $< @WIN32    ###
	$(CRES) $< -o $@
	@echo ###     RESOURCE@WIN32 BUILT      ###
	@echo.

# Build Win32 and Linux release
$(MAKECMDGOALS): $(BUILD_DIR)\$(WIN_DIR) $(BUILD_DIR)\$(LINUX_DIR) $(OBJS_WIN) $(OBJS_LIN)
	@echo ### RELEASE $@ BUILDING PROCEDURE ###
	@echo ###       MAKE RELEASE DIR        ###
	@echo.
	-mkdir $(RELEASE_DIR)\$@
	@echo.
	@echo ###    BUILDING WIN32 RELEASE     ###
	@echo.
	$(CXX) $(OBJS_WIN) -o $(RELEASE_DIR)\$@\$@_win32.exe $(LIBRARIES_INCLUDE:%=-%)
	@echo.
	@echo ###    BUILDING LINUX RELEASE     ###
	@echo.
	$(BASH)	-c "$(CXX) $(OBJS_LIN) -o $(RELEASE_DIR)/$@/$@_linux.exe $(LIBRARIES_INCLUDE:%=-%)"
	@echo.
	@echo ### RELEASE $@ FINISHED BUILDING  ###

# Makedir Release dir
$(RELEASE_DIR):
	mkdir $(RELEASE_DIR)

# Clear bin folder
.PHONY: clean
clean:
	@echo ###    CLEAN BUILD DIRECTORIES    ###
	@echo.
	del /q /f $(BUILD_DIR)\$(WIN_DIR)\*
	del /q /f $(BUILD_DIR)\$(LINUX_DIR)\*
	@echo.
	@echo ###              DONE             ###