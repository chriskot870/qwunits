add_library(direction_units OBJECT
  direction.cpp
  degrees.cpp
  radians.cpp
  compass_heading.cpp
)

#
# Add this directory to the list of directories to look for include files
#
target_include_directories(direction_units PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}/src
)

#
# Use the C++23 option
# Done by add_compile_options higher up
#target_compile_options(direction_units PUBLIC -std=c++23)

#add_subdirectory(tests)