add_library(distance_units OBJECT
  millimeter.cpp
  inches.cpp
  distance.cpp
)

#
# Add this directory to the list of directories to look for include files
#
set(DISTANCE_HEADERS_INSTALL_DIR include/units/humidity)
target_include_directories(distance_units PRIVATE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src>
  $<INSTALL_INTERFACE:${DISTANCE_HEADERS_INSTALL_DIR}>
)

install(
  FILES include/distance.h
        include/inches.h
        include/millimeter.h
  DESTINATION ${DISTANCE_HEADERS_INSTALL_DIR}
  COMPONENT dev
)


#
# Use the C++23 option
# Done higher up by add_compile_options
#target_compile_options(distance_units PUBLIC -std=c++23)
