add_library(direction_units OBJECT
  direction.cpp
  degrees.cpp
  radians.cpp
  compass_heading.cpp
)

#
# Add this directory to the list of directories to look for include files
#
set(DIRECTION_HEADERS_INSTALL_DIR direction/include)
target_include_directories(direction_units PRIVATE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src>
  $<INSTALL_INTERFACE:${DIRECTION_HEADERS_INSTALL_DIR}>
)

install(
  FILES include/direction.h
        include/degrees.h
        include/radians.h
        include/compass_heading.h
  DESTINATION ${DIRECTION_HEADERS_INSTALL_DIR}
  COMPONENT dev
)

#
# Use the C++23 option
# Done by add_compile_options higher up
#target_compile_options(direction_units PUBLIC -std=c++23)

#add_subdirectory(tests)