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
set(SPEED_HEADERS_INSTALL_DIR include/qw/units/speed/include)
target_include_directories(speed_units PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src>
  $<INSTALL_INTERFACE:${SPEED_HEADERS_INSTALL_DIR}>
)

install(
  FILES include/speed.h
        include/miles_per_hour.h
        include/kilometers_per_hour.h
        include/meters_per_second.h
        include/knots.h
  DESTINATION ${SPEED_HEADERS_INSTALL_DIR}
  COMPONENT dev
)

#
# Use the C++23 option
# Set higher up with add_compiler_option
#target_compile_options(speed_units PUBLIC -std=c++23)
