# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.21

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /Applications/CMake.app/Contents/bin/cmake

# The command to remove a file.
RM = /Applications/CMake.app/Contents/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/epico/Downloads/coro-micro-tp-etu-master/lab2

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/epico/Downloads/coro-micro-tp-etu-master/lab2/build

# Utility rule file for bin.

# Include any custom commands dependencies for this target.
include CMakeFiles/bin.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/bin.dir/progress.make

CMakeFiles/bin: lab2.bin

lab2.bin: lab2
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/Users/epico/Downloads/coro-micro-tp-etu-master/lab2/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Generating lab2.bin"
	/usr/local/bin/arm-none-eabi-objcopy -Obinary lab2 lab2.bin

bin: CMakeFiles/bin
bin: lab2.bin
bin: CMakeFiles/bin.dir/build.make
.PHONY : bin

# Rule to build all files generated by this target.
CMakeFiles/bin.dir/build: bin
.PHONY : CMakeFiles/bin.dir/build

CMakeFiles/bin.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/bin.dir/cmake_clean.cmake
.PHONY : CMakeFiles/bin.dir/clean

CMakeFiles/bin.dir/depend:
	cd /Users/epico/Downloads/coro-micro-tp-etu-master/lab2/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/epico/Downloads/coro-micro-tp-etu-master/lab2 /Users/epico/Downloads/coro-micro-tp-etu-master/lab2 /Users/epico/Downloads/coro-micro-tp-etu-master/lab2/build /Users/epico/Downloads/coro-micro-tp-etu-master/lab2/build /Users/epico/Downloads/coro-micro-tp-etu-master/lab2/build/CMakeFiles/bin.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/bin.dir/depend

