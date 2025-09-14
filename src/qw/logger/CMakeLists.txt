
add_compile_options(-fPIC -std=c++23)

# set_target_properties(MyStaticLib PROPERTIES POSITION_INDEPENDENT_CODE TRUE)
set(QW_LOGGER_LIB qwlogger)

add_library(${QW_LOGGER_LIB} SHARED
  logger.cpp
)

# Also make a static library for dev package
add_library(${QW_LOGGER_LIB}_static STATIC
  logger.cpp
)

find_package(fmt REQUIRED)

#
# Add this directory to the list of directories to look for include files
#
set(LOGGER_HEADERS_INSTALL_DIR include/qw/units/direction/include)
target_include_directories(${QW_LOGGER_LIB} PRIVATE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src>
  $<INSTALL_INTERFACE:${LOGGER_HEADERS_INSTALL_DIR}>
)


# Install measurement header file for dev package
install(
  FILES include/logger.h.h
  DESTINATION include/qw/logger/include/logger.h
  COMPONENT dev
)
# Install static library for dev package
install(
  TARGETS ${QW_LOGGER_LIB}_static
  DESTINATION lib/qw/logger/lib/lib${QW_LOGGER_LIB}.a
  COMPONENT dev
)
# Install shared library for runtime package
install(
  TARGETS ${QW_LOGGER_LIB}
  DESTINATION lib/qw/logger/lib
  COMPONENT runtime
  )

