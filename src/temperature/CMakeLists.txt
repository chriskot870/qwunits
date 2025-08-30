add_library(temperature_units OBJECT
  temperature.cpp
  celsius.cpp
  fahrenheit.cpp
  kelvin.cpp
)

#
# Add this directory to the list of directories to look for include files
#
target_include_directories(temperature_units PRIVATE
  ${CMAKE_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}
)

#
# Use the C++23 option
# Set higher with add_compile_options
#target_compile_options(temperature_units PUBLIC -std=c++23)

#add_subdirectory(tests)
