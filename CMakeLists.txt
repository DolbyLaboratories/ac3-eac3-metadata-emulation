# Almost all CMake files should start with this
# You should always specify a range with the newest
# and oldest tested versions of CMake. This will ensure
# you pick up the best policies.
cmake_minimum_required(VERSION 3.15...4.0)

# This is your project statement. You should always list languages;
# Listing the version is nice here since it sets lots of useful variables
project(
  MdEmul
  VERSION 1.0
  LANGUAGES C CXX)

# If you set any CMAKE_ variables, that can go here.
# (But usually don't do this, except maybe for C++ standard)

# Find packages go here.

find_package(SndFile REQUIRED)

# You should usually split this into folders, but this is a simple example

# This is a "default" library, and will match the *** variable setting.
# Other common choices are STATIC, SHARED, and MODULE
# Including header files here helps IDEs but is not required.
# Output libname matches target name, with the usual extensions on your system

add_library(MdEmulLib STATIC src/dd_emulation.c src/dlb_md_emul_api.c src/drc_applier.c src/emul_filters.c src/md_compr.c src/dd_emulation.h src/dlb_md_emul_pvt.h src/drc_applier.h src/emul_filters.h src/md_compr.h)

include_directories(dlb_intrinsics dlb_intrinsics/backend/generic include)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DDLB_BACKEND_GENERIC_FLOAT64")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DDLB_BACKEND_GENERIC_FLOAT64")

# Link each target with other targets or add options, etc.

# Adding something we can run - Output name matches target name
add_executable(MdEmu metadata_emulation/metadata_emulation_main.cpp)

add_executable(graph_check tools/src/graph_check.c)

# Make sure you link your targets with this command. It can also link libraries and
# even flags, so linking a target that does not exist will not give a configure-time error.
target_link_libraries(MdEmu PRIVATE MdEmulLib SndFile::sndfile)
