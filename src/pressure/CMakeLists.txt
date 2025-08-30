add_library(pressure_units OBJECT
  pressure.cpp
  inches_mercury.cpp
  millibar.cpp
)

#
# Add this directory to the list of directories to look for include files
#
target_include_directories(pressure_units PRIVATE
  ${CMAKE_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}
)

#
# Use the C++23 option
# Set higher up with add_compile_options
#target_compile_options(pressure_units PUBLIC -std=c++23)

# Add the test directory
#add_subdirectory(tests)
