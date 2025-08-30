add_library(light_units OBJECT
  lux.cpp
  klux.cpp
  foot_candle.cpp
  light.cpp
)

#
# Add this directory to the list of directories to look for include files
#
target_include_directories(light_units PRIVATE
  ${CMAKE_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}
)

#
# Use the C++23 option
# Done higher up with add_compiler_options
#target_compile_options(light_units PUBLIC -std=c++23)