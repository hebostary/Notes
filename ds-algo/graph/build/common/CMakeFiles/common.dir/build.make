# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list

# Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The program to use to edit the cache.
CMAKE_EDIT_COMMAND = /usr/bin/ccmake

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/workspace/Notes/ds-algo/graph/src

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/workspace/Notes/ds-algo/graph/build

# Include any dependencies generated for this target.
include common/CMakeFiles/common.dir/depend.make

# Include the progress variables for this target.
include common/CMakeFiles/common.dir/progress.make

# Include the compile flags for this target's objects.
include common/CMakeFiles/common.dir/flags.make

common/CMakeFiles/common.dir/common.cpp.o: common/CMakeFiles/common.dir/flags.make
common/CMakeFiles/common.dir/common.cpp.o: /home/workspace/Notes/ds-algo/graph/src/common/common.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/workspace/Notes/ds-algo/graph/build/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object common/CMakeFiles/common.dir/common.cpp.o"
	cd /home/workspace/Notes/ds-algo/graph/build/common && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/common.dir/common.cpp.o -c /home/workspace/Notes/ds-algo/graph/src/common/common.cpp

common/CMakeFiles/common.dir/common.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/common.dir/common.cpp.i"
	cd /home/workspace/Notes/ds-algo/graph/build/common && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/workspace/Notes/ds-algo/graph/src/common/common.cpp > CMakeFiles/common.dir/common.cpp.i

common/CMakeFiles/common.dir/common.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/common.dir/common.cpp.s"
	cd /home/workspace/Notes/ds-algo/graph/build/common && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/workspace/Notes/ds-algo/graph/src/common/common.cpp -o CMakeFiles/common.dir/common.cpp.s

common/CMakeFiles/common.dir/common.cpp.o.requires:
.PHONY : common/CMakeFiles/common.dir/common.cpp.o.requires

common/CMakeFiles/common.dir/common.cpp.o.provides: common/CMakeFiles/common.dir/common.cpp.o.requires
	$(MAKE) -f common/CMakeFiles/common.dir/build.make common/CMakeFiles/common.dir/common.cpp.o.provides.build
.PHONY : common/CMakeFiles/common.dir/common.cpp.o.provides

common/CMakeFiles/common.dir/common.cpp.o.provides.build: common/CMakeFiles/common.dir/common.cpp.o

# Object files for target common
common_OBJECTS = \
"CMakeFiles/common.dir/common.cpp.o"

# External object files for target common
common_EXTERNAL_OBJECTS =

common/libcommon.a: common/CMakeFiles/common.dir/common.cpp.o
common/libcommon.a: common/CMakeFiles/common.dir/build.make
common/libcommon.a: common/CMakeFiles/common.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX static library libcommon.a"
	cd /home/workspace/Notes/ds-algo/graph/build/common && $(CMAKE_COMMAND) -P CMakeFiles/common.dir/cmake_clean_target.cmake
	cd /home/workspace/Notes/ds-algo/graph/build/common && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/common.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
common/CMakeFiles/common.dir/build: common/libcommon.a
.PHONY : common/CMakeFiles/common.dir/build

common/CMakeFiles/common.dir/requires: common/CMakeFiles/common.dir/common.cpp.o.requires
.PHONY : common/CMakeFiles/common.dir/requires

common/CMakeFiles/common.dir/clean:
	cd /home/workspace/Notes/ds-algo/graph/build/common && $(CMAKE_COMMAND) -P CMakeFiles/common.dir/cmake_clean.cmake
.PHONY : common/CMakeFiles/common.dir/clean

common/CMakeFiles/common.dir/depend:
	cd /home/workspace/Notes/ds-algo/graph/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/workspace/Notes/ds-algo/graph/src /home/workspace/Notes/ds-algo/graph/src/common /home/workspace/Notes/ds-algo/graph/build /home/workspace/Notes/ds-algo/graph/build/common /home/workspace/Notes/ds-algo/graph/build/common/CMakeFiles/common.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : common/CMakeFiles/common.dir/depend

