#
# Add the devices sub-directory
#

add_compile_options(-fPIC -std=c++23)

# set_target_properties(MyStaticLib PROPERTIES POSITION_INDEPENDENT_CODE TRUE)
set(QW_WEATHER_DEVICES_LIB weatherdevices)

# Build all the objects one time to be linked into both shared and static libraries
add_library(${QW_WEATHER_DEVICES_LIB}_objects OBJECT
  anemometer_adafruit.cpp
)

# Link the objects into a shared library
add_library(${QW_WEATHER_DEVICES_LIB}_shared SHARED 
  $<TARGET_OBJECTS:${QW_WEATHER_DEVICES_LIB}_objects>
)
# We don't want _shared in the shared library name
set_target_properties(${QW_WEATHER_DEVICES_LIB}_shared PROPERTIES OUTPUT_NAME ${QW_WEATHER_DEVICES_LIB})

# Link the objects into a static library
add_library(${QW_WEATHER_DEVICES_LIB}_static STATIC
  $<TARGET_OBJECTS:${QW_WEATHER_DEVICES_LIB}_objects>
)
# We want the static library to have the same name as the shared library
set_target_properties(${QW_WEATHER_DEVICES_LIB}_static PROPERTIES OUTPUT_NAME ${QW_WEATHER_DEVICES_LIB})

find_package(fmt REQUIRED)

target_link_libraries(${QW_WEATHER_DEVICES_LIB}_shared PUBLIC
  qwunits
)
target_link_libraries(${QW_WEATHER_DEVICES_LIB}_static PUBLIC
  qwunits
)

# define installation directory for header files
set(WEATHER_DEVICES_HEADERS_INSTALL_DIR include/qw/devices/include)
#
# Add this directory to the list of directories to look for include files
# Add directories for shared library target
target_include_directories(${QW_WEATHER_DEVICES_LIB}_objects PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}>
  $<INSTALL_INTERFACE:${WEATHER_DEVICES_HEADERS_INSTALL_DIR}>
)

# Set things up for the ecowitt library
# set_target_properties(MyStaticLib PROPERTIES POSITION_INDEPENDENT_CODE TRUE)
set(QW_ECOWITT_LN90LP_DEVICE_LIB ecowittln90lp)

add_library(${QW_ECOWITT_LN90LP_DEVICE_LIB}_objects OBJECT
  ecowitt_ln90lp.cpp
)
# Link the objects into a shared library
add_library(${QW_ECOWITT_LN90LP_DEVICE_LIB}_shared SHARED
  $<TARGET_OBJECTS:${QW_ECOWITT_LN90LP_DEVICE_LIB}_objects>
)
# Set the shared library name without the _shared suffix
set_target_properties(${QW_ECOWITT_LN90LP_DEVICE_LIB}_shared PROPERTIES OUTPUT_NAME ${QW_ECOWITT_LN90LP_DEVICE_LIB})

# Link the objects into a static library
add_library(${QW_ECOWITT_LN90LP_DEVICE_LIB}_static STATIC
  $<TARGET_OBJECTS:${QW_ECOWITT_LN90LP_DEVICE_LIB}_objects>
)
# We want the static library to have the same name as the shared library
set_target_properties(${QW_ECOWITT_LN90LP_DEVICE_LIB}_static PROPERTIES OUTPUT_NAME ${QW_ECOWITT_LN90LP_DEVICE_LIB})

find_package(fmt REQUIRED)
target_link_libraries(${QW_ECOWITT_LN90LP_DEVICE_LIB}_shared PUBLIC
  qwunits
  modbus
)
target_link_libraries(${QW_ECOWITT_LN90LP_DEVICE_LIB}_static PUBLIC
  qwunits
  modbus
)

# define installation directory for header files
set(ECOWITT_LN90LP_DEVICE_HEADERS_INSTALL_DIR include/qw/devices/include)
#
# Add this directory to the list of directories to look for include files
# Add directories for shared library target
target_include_directories(${QW_ECOWITT_LN90LP_DEVICE_LIB}_objects PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}>
  $<INSTALL_INTERFACE:${ECOWITT_LN90LP_DEVICE_HEADERS_INSTALL_DIR}>
)

# Install measurement header file for dev package
install( FILES
    include/anemometer_adafruit.h
  DESTINATION include/qw/devices
  COMPONENT weatherdevices_dev
)
# Install static library for dev package
install(
  TARGETS ${QW_WEATHER_DEVICES_LIB}_static
  DESTINATION lib/qw/devices
  COMPONENT weatherdevices_dev
)
# Install shared library for runtime package
install(
  TARGETS ${QW_WEATHER_DEVICES_LIB}_shared
  DESTINATION lib/qw/devices/lib
  COMPONENT weatherdevices_runtime
  )

install( FILES
    include/ecowitt_ln90lp.h
  DESTINATION include/qw/devices
  COMPONENT ecowittln90lp_dev
)
# Install static library for dev package
install(
  TARGETS ${QW_ECOWITT_LN90LP_DEVICE_LIB}_static
  DESTINATION lib/qw/devices/
  COMPONENT ecowittln90lp_dev
)
# Install shared library for runtime package
install(
  TARGETS ${QW_ECOWITT_LN90LP_DEVICE_LIB}_shared
  DESTINATION lib/qw/devices/lib
  COMPONENT ecowittln90lp_runtime
  )

add_subdirectory(i2c)
