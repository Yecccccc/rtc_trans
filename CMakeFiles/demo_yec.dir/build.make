# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.26

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
CMAKE_COMMAND = /home/yec/.local/lib/python3.8/site-packages/cmake/data/bin/cmake

# The command to remove a file.
RM = /home/yec/.local/lib/python3.8/site-packages/cmake/data/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/yec/Desktop/mycode

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/yec/Desktop/mycode

# Include any dependencies generated for this target.
include CMakeFiles/demo_yec.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/demo_yec.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/demo_yec.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/demo_yec.dir/flags.make

CMakeFiles/demo_yec.dir/trans.cpp.o: CMakeFiles/demo_yec.dir/flags.make
CMakeFiles/demo_yec.dir/trans.cpp.o: trans.cpp
CMakeFiles/demo_yec.dir/trans.cpp.o: CMakeFiles/demo_yec.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/yec/Desktop/mycode/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/demo_yec.dir/trans.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/demo_yec.dir/trans.cpp.o -MF CMakeFiles/demo_yec.dir/trans.cpp.o.d -o CMakeFiles/demo_yec.dir/trans.cpp.o -c /home/yec/Desktop/mycode/trans.cpp

CMakeFiles/demo_yec.dir/trans.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/demo_yec.dir/trans.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/yec/Desktop/mycode/trans.cpp > CMakeFiles/demo_yec.dir/trans.cpp.i

CMakeFiles/demo_yec.dir/trans.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/demo_yec.dir/trans.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/yec/Desktop/mycode/trans.cpp -o CMakeFiles/demo_yec.dir/trans.cpp.s

# Object files for target demo_yec
demo_yec_OBJECTS = \
"CMakeFiles/demo_yec.dir/trans.cpp.o"

# External object files for target demo_yec
demo_yec_EXTERNAL_OBJECTS =

demo_yec: CMakeFiles/demo_yec.dir/trans.cpp.o
demo_yec: CMakeFiles/demo_yec.dir/build.make
demo_yec: /opt/ffmpeg/lib/libavutil.so
demo_yec: /opt/ffmpeg/lib/libswresample.so
demo_yec: /opt/ffmpeg/lib/libavcodec.so
demo_yec: /opt/ffmpeg/lib/libavfilter.so
demo_yec: /opt/ffmpeg/lib/libswscale.so
demo_yec: /opt/ffmpeg/lib/libavformat.so
demo_yec: CMakeFiles/demo_yec.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/yec/Desktop/mycode/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable demo_yec"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/demo_yec.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/demo_yec.dir/build: demo_yec
.PHONY : CMakeFiles/demo_yec.dir/build

CMakeFiles/demo_yec.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/demo_yec.dir/cmake_clean.cmake
.PHONY : CMakeFiles/demo_yec.dir/clean

CMakeFiles/demo_yec.dir/depend:
	cd /home/yec/Desktop/mycode && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/yec/Desktop/mycode /home/yec/Desktop/mycode /home/yec/Desktop/mycode /home/yec/Desktop/mycode /home/yec/Desktop/mycode/CMakeFiles/demo_yec.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/demo_yec.dir/depend

