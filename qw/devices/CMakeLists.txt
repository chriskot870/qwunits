#
# Add the devices sub-directory
#

add_compile_options(-fPIC -std=c++23)

# set_target_properties(MyStaticLib PROPERTIES POSITION_INDEPENDENT_CODE TRUE)
set(QW_WEATHER_DEVICES_LIB weatherdevices)

add_library(${QW_WEATHER_DEVICES_LIB} SHARED
  anemometer_adafruit.cpp
  ecowitt_ln90lp.cpp
)

# Also make a static library for dev package
add_library(${QW_WEATHER_DEVICES_LIB}_static STATIC
  anemometer_adafruit.cpp
  ecowitt_ln90lp.cpp
)

find_package(fmt REQUIRED)

target_link_libraries(${QW_WEATHER_DEVICES_LIB} PUBLIC
  qwunits
  modbus
)

# define installation directory for header files
set(WEATHER_DEVICES_HEADERS_INSTALL_DIR include/qw/devices/include)
#
# Add this directory to the list of directories to look for include files
# Add directories for shared library target
target_include_directories(${QW_WEATHER_DEVICES_LIB} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}>
  $<INSTALL_INTERFACE:${WEATHER_DEVICES_HEADERS_INSTALL_DIR}>
)
# Add same directoris for static library target
target_include_directories(${QW_WEATHER_DEVICES_LIB}_static PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}>
  $<INSTALL_INTERFACE:${WEATHER_HEADERS_INSTALL_DIR}>
)

# Install measurement header file for dev package
install( FILES
    include/anonemometer_adafruit.h
    include/ecowitt_ln90lp.h
  DESTINATION include/qw/devices
  COMPONENT dev
)
# Install static library for dev package
install(
  TARGETS ${QW_WEATHER_DEVICES_LIB}_static
  DESTINATION lib/qw/devices/i2c/lib/lib${QW_WEATHER_DEVICES_LIB}.a
  COMPONENT dev
)
# Install shared library for runtime package
install(
  TARGETS ${QW_WEATHER_DEVICES_LIB}
  DESTINATION lib/qw/devices/lib
  COMPONENT runtime
  )

add_subdirectory(i2c)
