#
# This is the top-level CmakeLists.txt file
#
cmake_minimum_required(VERSION 3.28.0)

#
# Need to set this variable prior to project
# WE let presets set this
#set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/cmake/toolchain.cmake)

# set the prefix to /usr/lib/qw/units
# Libraries will be installed in /usr/lib/qw/units/lib
# This needs to be done prior to defining the project
# This gets done in cpack_debian.cmake via CPACK_PACKAGING_INSTALL_PREFIX
#set(CMAKE_INSTALL_PREFIX  "/usr/lib/qw/units") 

#
# Set the project name and the compilers to be used
#
project(qwunits CXX)

#enable_testing()
#
# I needed to include Dart or else I got a could not find DartConfig.tcl error.
# I have read some docs that Dart was removed from Cmake and is deprecated.
# It says to use the include Dart to add it to Cmake. I am a bit confused
# about this. I have not found any documentation that explains this.
#
#include(Dart)  # This prevents a Dartconfiguration.tcl file not found error

#add_library(qwunits SHARED)
#
# Add the sub-directories to build
#

add_subdirectory(src)

#
# The include(Cpack) creates a default package build. In order to control
# the CPACK settings you need to define a CPACK_PROPERTIES_FILE prior to
# including CPack. WHen you include CPack the CMake system creates a
# CPackConfig.cmake file in the build directory. These have default settings.
# At the end of that file there is an include(${CPACK+PROPERTIES_FILES}).
# This allows you to change any default setting and add others.
# It doesn't seem like there needs to be a Cpack.cmkae file at the top
# level. Just having include(CPcak) seem to be adequate.
# Be sure and set PACK_PROPERTIES_FILE pror to including CPack.
#
# After a build you can cd to build and run cpack and it should build
# a package for you.
#
# set(CPACK_PROPERTIES_FILE ${CMAKE_SOURCE_DIR}/cmake/cpack_debian.cmake)
#set(CPACK_GENERATOR "DEB")
include(${CMAKE_SOURCE_DIR}/cmake/cpack_debian.cmake)
include(CPack)
