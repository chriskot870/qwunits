add_library(speed_units OBJECT
  speed.cpp
  miles_per_hour.cpp
  kilometers_per_hour.cpp
  meters_per_second.cpp
  knots.cpp
)

#
# Add this directory to the list of directories to look for include files
#
target_include_directories(speed_units PRIVATE
  ${CMAKE_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}
)


#
# Use the C++23 option
# Set higher up with add_compiler_option
#target_compile_options(speed_units PUBLIC -std=c++23)
