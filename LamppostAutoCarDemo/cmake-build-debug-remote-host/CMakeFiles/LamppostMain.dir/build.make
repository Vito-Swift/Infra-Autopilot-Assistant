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
CMAKE_COMMAND = /usr/local/bin/cmake

# The command to remove a file.
RM = /usr/local/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /tmp/tmp.3SNubaVh5g

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /tmp/tmp.3SNubaVh5g/cmake-build-debug-remote-host

# Include any dependencies generated for this target.
include CMakeFiles/LamppostMain.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/LamppostMain.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/LamppostMain.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/LamppostMain.dir/flags.make

CMakeFiles/LamppostMain.dir/src/LP_Main.cc.o: CMakeFiles/LamppostMain.dir/flags.make
CMakeFiles/LamppostMain.dir/src/LP_Main.cc.o: ../src/LP_Main.cc
CMakeFiles/LamppostMain.dir/src/LP_Main.cc.o: CMakeFiles/LamppostMain.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/tmp/tmp.3SNubaVh5g/cmake-build-debug-remote-host/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/LamppostMain.dir/src/LP_Main.cc.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/LamppostMain.dir/src/LP_Main.cc.o -MF CMakeFiles/LamppostMain.dir/src/LP_Main.cc.o.d -o CMakeFiles/LamppostMain.dir/src/LP_Main.cc.o -c /tmp/tmp.3SNubaVh5g/src/LP_Main.cc

CMakeFiles/LamppostMain.dir/src/LP_Main.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/LamppostMain.dir/src/LP_Main.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /tmp/tmp.3SNubaVh5g/src/LP_Main.cc > CMakeFiles/LamppostMain.dir/src/LP_Main.cc.i

CMakeFiles/LamppostMain.dir/src/LP_Main.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/LamppostMain.dir/src/LP_Main.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /tmp/tmp.3SNubaVh5g/src/LP_Main.cc -o CMakeFiles/LamppostMain.dir/src/LP_Main.cc.s

CMakeFiles/LamppostMain.dir/src/LP_CommPanel.cc.o: CMakeFiles/LamppostMain.dir/flags.make
CMakeFiles/LamppostMain.dir/src/LP_CommPanel.cc.o: ../src/LP_CommPanel.cc
CMakeFiles/LamppostMain.dir/src/LP_CommPanel.cc.o: CMakeFiles/LamppostMain.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/tmp/tmp.3SNubaVh5g/cmake-build-debug-remote-host/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/LamppostMain.dir/src/LP_CommPanel.cc.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/LamppostMain.dir/src/LP_CommPanel.cc.o -MF CMakeFiles/LamppostMain.dir/src/LP_CommPanel.cc.o.d -o CMakeFiles/LamppostMain.dir/src/LP_CommPanel.cc.o -c /tmp/tmp.3SNubaVh5g/src/LP_CommPanel.cc

CMakeFiles/LamppostMain.dir/src/LP_CommPanel.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/LamppostMain.dir/src/LP_CommPanel.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /tmp/tmp.3SNubaVh5g/src/LP_CommPanel.cc > CMakeFiles/LamppostMain.dir/src/LP_CommPanel.cc.i

CMakeFiles/LamppostMain.dir/src/LP_CommPanel.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/LamppostMain.dir/src/LP_CommPanel.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /tmp/tmp.3SNubaVh5g/src/LP_CommPanel.cc -o CMakeFiles/LamppostMain.dir/src/LP_CommPanel.cc.s

# Object files for target LamppostMain
LamppostMain_OBJECTS = \
"CMakeFiles/LamppostMain.dir/src/LP_Main.cc.o" \
"CMakeFiles/LamppostMain.dir/src/LP_CommPanel.cc.o"

# External object files for target LamppostMain
LamppostMain_EXTERNAL_OBJECTS =

LamppostMain: CMakeFiles/LamppostMain.dir/src/LP_Main.cc.o
LamppostMain: CMakeFiles/LamppostMain.dir/src/LP_CommPanel.cc.o
LamppostMain: CMakeFiles/LamppostMain.dir/build.make
LamppostMain: libDRIVER_LIB.so
LamppostMain: /usr/lib/libwiringPi.so
LamppostMain: CMakeFiles/LamppostMain.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/tmp/tmp.3SNubaVh5g/cmake-build-debug-remote-host/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX executable LamppostMain"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/LamppostMain.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/LamppostMain.dir/build: LamppostMain
.PHONY : CMakeFiles/LamppostMain.dir/build

CMakeFiles/LamppostMain.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/LamppostMain.dir/cmake_clean.cmake
.PHONY : CMakeFiles/LamppostMain.dir/clean

CMakeFiles/LamppostMain.dir/depend:
	cd /tmp/tmp.3SNubaVh5g/cmake-build-debug-remote-host && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /tmp/tmp.3SNubaVh5g /tmp/tmp.3SNubaVh5g /tmp/tmp.3SNubaVh5g/cmake-build-debug-remote-host /tmp/tmp.3SNubaVh5g/cmake-build-debug-remote-host /tmp/tmp.3SNubaVh5g/cmake-build-debug-remote-host/CMakeFiles/LamppostMain.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/LamppostMain.dir/depend

