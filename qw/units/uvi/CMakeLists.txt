add_library(uvi_units OBJECT
    uvi.cpp
    )

#
# Add this directory to the list of directories to look for include files
#
set(UVI_HEADERS_INSTALL_DIR include/qw/units/uvi/include)
target_include_directories(temperature_units PUBLIC
  $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:${UVI_HEADERS_INSTALL_DIR}>
)

install(
  FILES include/uvi.h
  DESTINATION ${UVI_HEADERS_INSTALL_DIR}
  COMPONENT dev
)

#
# Use the C++23 option
# Set higher up using add_complier_options
#target_compile_options(uvi_units PUBLIC -std=c++23)
