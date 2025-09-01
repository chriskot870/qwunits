add_library(pressure_units OBJECT
  pressure.cpp
  inches_mercury.cpp
  millibar.cpp
)

#
# Add this directory to the list of directories to look for include files
#
set(PRESSURE_HEADERS_INSTALL_DIR pressure/include)
target_include_directories(pressure_units PRIVATE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src>
  $<INSTALL_INTERFACE:${PRESSURE_HEADERS_INSTALL_DIR}>
)

install(
  FILES include/pressure.h
        include/millibar.h
        include/inches_mercury.h
  DESTINATION ${PRESSURE_HEADERS_INSTALL_DIR}
  COMPONENT dev
)

#
# Use the C++23 option
# Set higher up with add_compile_options
#target_compile_options(pressure_units PUBLIC -std=c++23)

# Add the test directory
#add_subdirectory(tests)
