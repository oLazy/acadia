# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.20

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
CMAKE_COMMAND = /snap/clion/164/bin/cmake/linux/bin/cmake

# The command to remove a file.
RM = /snap/clion/164/bin/cmake/linux/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/eric/Projects/acadia_interview

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/eric/Projects/acadia_interview/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/acadia_interview.dir/depend.make
# Include the progress variables for this target.
include CMakeFiles/acadia_interview.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/acadia_interview.dir/flags.make

CMakeFiles/acadia_interview.dir/main.cpp.o: CMakeFiles/acadia_interview.dir/flags.make
CMakeFiles/acadia_interview.dir/main.cpp.o: ../main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/eric/Projects/acadia_interview/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/acadia_interview.dir/main.cpp.o"
	/usr/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/acadia_interview.dir/main.cpp.o -c /home/eric/Projects/acadia_interview/main.cpp

CMakeFiles/acadia_interview.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/acadia_interview.dir/main.cpp.i"
	/usr/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/eric/Projects/acadia_interview/main.cpp > CMakeFiles/acadia_interview.dir/main.cpp.i

CMakeFiles/acadia_interview.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/acadia_interview.dir/main.cpp.s"
	/usr/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/eric/Projects/acadia_interview/main.cpp -o CMakeFiles/acadia_interview.dir/main.cpp.s

# Object files for target acadia_interview
acadia_interview_OBJECTS = \
"CMakeFiles/acadia_interview.dir/main.cpp.o"

# External object files for target acadia_interview
acadia_interview_EXTERNAL_OBJECTS =

acadia_interview: CMakeFiles/acadia_interview.dir/main.cpp.o
acadia_interview: CMakeFiles/acadia_interview.dir/build.make
acadia_interview: CMakeFiles/acadia_interview.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/eric/Projects/acadia_interview/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable acadia_interview"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/acadia_interview.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/acadia_interview.dir/build: acadia_interview
.PHONY : CMakeFiles/acadia_interview.dir/build

CMakeFiles/acadia_interview.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/acadia_interview.dir/cmake_clean.cmake
.PHONY : CMakeFiles/acadia_interview.dir/clean

CMakeFiles/acadia_interview.dir/depend:
	cd /home/eric/Projects/acadia_interview/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/eric/Projects/acadia_interview /home/eric/Projects/acadia_interview /home/eric/Projects/acadia_interview/cmake-build-debug /home/eric/Projects/acadia_interview/cmake-build-debug /home/eric/Projects/acadia_interview/cmake-build-debug/CMakeFiles/acadia_interview.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/acadia_interview.dir/depend

