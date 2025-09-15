#
# CMakeLists.txt file for the i2c devices
#

add_compile_options(-fPIC -std=c++23)

# set_target_properties(MyStaticLib PROPERTIES POSITION_INDEPENDENT_CODE TRUE)
set(QW_I2C_DEVICES_LIB i2cdevices)

add_library(${QW_I2C_DEVICES_LIB} SHARED
  i2cbus.cpp
  sht4x.cpp
  lps22.cpp
  ads1015.cpp
)

# Also make a static library for dev package
add_library(${QW_I2C_DEVICES_LIB}_static STATIC
  i2cbus.cpp
  sht4x.cpp
  lps22.cpp
  ads1015.cpp
)

find_package(fmt REQUIRED)

target_link_libraries(${QW_I2C_DEVICES_LIB} PUBLIC
  temperature_units
  pressure_units
  humidity_units
)

# define installation directory for header files
set(I2C_DEVICES_HEADERS_INSTALL_DIR include/qw/devices/i2c/include)
#
# Add this directory to the list of directories to look for include files
# Add directories for shared library target
target_include_directories(${QW_I2C_DEVICES_LIB} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}>
  $<INSTALL_INTERFACE:${I2C_DEVICES_HEADERS_INSTALL_DIR}>
)
# Add same directoris for static library target
target_include_directories(${QW_I2C_DEVICES_LIB}_static PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}>
  $<INSTALL_INTERFACE:${I2C_DEVICES_HEADERS_INSTALL_DIR}>
)

# Install measurement header file for dev package
install( FILES
    include/ads1015.h
    include/i2cbus.h
    include/lps22.h
    include/sht4x.h
  DESTINATION include/qw/devices/i2c
  COMPONENT dev
)
# Install static library for dev package
install(
  TARGETS ${QW_I2C_DEVICES_LIB}_static
  DESTINATION lib/qw/devices/i2c/lib/lib${QW_I2C_DEVICES_LIB}.a
  COMPONENT dev
)
# Install shared library for runtime package
install(
  TARGETS ${QW_GPIO_LIB}
  DESTINATION lib/qw/devices/i2c/lib
  COMPONENT runtime
  )
