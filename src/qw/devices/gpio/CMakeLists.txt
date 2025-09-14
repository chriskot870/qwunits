
add_compile_options(-fPIC -std=c++23)

# set_target_properties(MyStaticLib PROPERTIES POSITION_INDEPENDENT_CODE TRUE)
set(QW_GPIO_LIB qwgpio)

add_library(${QW_GPIO_LIB} SHARED
  qwgpio_chip.cpp
  qwgpio_group.cpp
)

# Also make a static library for dev package
add_library(${QW_GPIO_LIB}_static STATIC
  qwgpio_chip.cpp
  qwgpio_group.cpp
)

find_package(fmt REQUIRED)

# define installation directory for header files
set(GPIO_HEADERS_INSTALL_DIR include/qw/devices/gpio/include)
#
# Add this directory to the list of directories to look for include files
# Add directories for shared library target
target_include_directories(${QW_GPIO_LIB} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src>
  $<INSTALL_INTERFACE:${GPIO_HEADERS_INSTALL_DIR}>
)
# Add same directoris for static library target
target_include_directories(${QW_GPIO_LIB}_static PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src>
  $<INSTALL_INTERFACE:${GPIO_HEADERS_INSTALL_DIR}>
)

# Install measurement header file for dev package
install( FILES
    include/qwgpio.h
    include/qwgpio_chip.h
    include/qwgpio_group.h
  DESTINATION include/qw/devices/gpio
  COMPONENT dev
)
# Install static library for dev package
install(
  TARGETS ${QW_GPIO_LIB}_static
  DESTINATION lib/qw/devices/gpio/lib/lib${QW_GPIO_LIB}.a
  COMPONENT dev
)
# Install shared library for runtime package
install(
  TARGETS ${QW_GPIO_LIB}
  DESTINATION lib/qw/devices/gpio/lib
  COMPONENT runtime
  )

