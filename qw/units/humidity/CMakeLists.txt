add_library(humidity_units OBJECT
    relative_humidity.cpp
    )

#
# Add this directory to the list of directories to look for include files
#
set(HUMIDITY_HEADERS_INSTALL_DIR include/qw/units/humidity/include)
target_include_directories(humidity_units PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}>
  $<INSTALL_INTERFACE:${HUMIDITY_HEADERS_INSTALL_DIR}>
)

install(
  FILES include/relative_humidity.h
  DESTINATION ${HUMIDITY_HEADERS_INSTALL_DIR}
  COMPONENT dev
)
#
# Use the C++23 option
# Done higher up with add_compile_options
#target_compile_options(humidity_units PUBLIC -std=c++23)
