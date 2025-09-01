add_library(temperature_units OBJECT
  temperature.cpp
  celsius.cpp
  fahrenheit.cpp
  kelvin.cpp
)

#
# Add this directory to the list of directories to look for include files
#
set(TEMPERATURE_HEADERS_INSTALL_DIR include/units/temperature)
target_include_directories(temperature_units PUBLIC
  $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:${TEMPERATURE_HEADERS_INSTALL_DIR}>
)

install(
  FILES include/temperature.h
        include/celsius.h
        include/fahrenheit.h
        include/kelvin.h
  DESTINATION ${TEMPERATURE_HEADERS_INSTALL_DIR}
  COMPONENT dev
)

#
# Use the C++23 option
# Set higher with add_compile_options
#target_compile_options(temperature_units PUBLIC -std=c++23)

#add_subdirectory(tests)
