add_library(light_units OBJECT
  lux.cpp
  klux.cpp
  foot_candle.cpp
  light.cpp
)

#
# Add this directory to the list of directories to look for include files
#
set(LIGHT_HEADERS_INSTALL_DIR include/qw/units/light/include)
target_include_directories(light_units PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src>
  $<INSTALL_INTERFACE:${LIGHT_HEADERS_INSTALL_DIR}>
)

install(
  FILES include/light.h
        include/lux.h
        include/klux.h
        include/foot_candle.h
  DESTINATION ${LIGHT_HEADERS_INSTALL_DIR}
  COMPONENT dev
)

#
# Use the C++23 option
# Done higher up with add_compiler_options
#target_compile_options(light_units PUBLIC -std=c++23)