add_library(distance_units OBJECT
  millimeter.cpp
  inches.cpp
  distance.cpp
)

#
# Add this directory to the list of directories to look for include files
#
target_include_directories(distance_units PRIVATE
  ${CMAKE_SOURCE_DIR}/src/lib
  ${CMAKE_CURRENT_SOURCE_DIR}
)

#
# Use the C++23 option
# Done higher up by add_compile_options
#target_compile_options(distance_units PUBLIC -std=c++23)
