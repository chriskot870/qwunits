


add_compile_options(-fPIC -std=c++23)

# set_target_properties(MyStaticLib PROPERTIES POSITION_INDEPENDENT_CODE TRUE)

add_library(${CMAKE_PROJECT_NAME} SHARED
    $<TARGET_OBJECTS:temperature_units>
    $<TARGET_OBJECTS:pressure_units>
    $<TARGET_OBJECTS:humidity_units>
    $<TARGET_OBJECTS:speed_units>
    $<TARGET_OBJECTS:distance_units>
    $<TARGET_OBJECTS:uvi_units>
    $<TARGET_OBJECTS:light_units>
    $<TARGET_OBJECTS:direction_units>
    )

# Also make a static library for dev package
add_library(${CMAKE_PROJECT_NAME}_static STATIC
    $<TARGET_OBJECTS:temperature_units>
    $<TARGET_OBJECTS:pressure_units>
    $<TARGET_OBJECTS:humidity_units>
    $<TARGET_OBJECTS:speed_units>
    $<TARGET_OBJECTS:distance_units>
    $<TARGET_OBJECTS:uvi_units>
    $<TARGET_OBJECTS:light_units>
    $<TARGET_OBJECTS:direction_units>
    )

#
# Add the target architecture to the library name by setting OUTPUT_NAME
#
#string(TOLOWER "${CMAKE_SYSTEM_PROCESSOR}" ARCH_NAME)
#
#set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES
#    OUTPUT_NAME "${CMAKE_PROJECT_NAME}_${ARCH_NAME}"
#)

add_subdirectory(temperature)
add_subdirectory(pressure)
add_subdirectory(humidity)
add_subdirectory(speed)
add_subdirectory(distance)
add_subdirectory(uvi)
add_subdirectory(light)
add_subdirectory(direction)

find_package(fmt REQUIRED)

set(MEASUREMENT_HEADERS_INSTALL_DIR include/units)

# Install measurement header file for dev package
install(
  FILES include/unit_measurement.h
  DESTINATION ${MEASUREMENT_HEADERS_INSTALL_DIR}
  COMPONENT dev
)
# Install static library for dev package
install(
  TARGETS ${CMAKE_PROJECT_NAME}_static
  DESTINATION lib/lib${CMAKE_PROJECT_NAME}.a
  COMPONENT dev
)
# Install shared library for runtime package
install(
  TARGETS ${CMAKE_PROJECT_NAME}
  DESTINATION lib
  COMPONENT runtime
  )
install(
  FILES "config/linux-qw.conf"
  DESTINATION "${CMAKE_INSTALL_SYSCONFDIR}/ld.so.conf.d"
  COMPONENT runtime
)

