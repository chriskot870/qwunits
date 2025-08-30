


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

add_subdirectory(temperature)
add_subdirectory(pressure)
add_subdirectory(humidity)
add_subdirectory(speed)
add_subdirectory(distance)
add_subdirectory(uvi)
add_subdirectory(light)
add_subdirectory(direction)

find_package(fmt REQUIRED)

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}/src
)

