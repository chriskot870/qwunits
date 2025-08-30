add_library(humidity_units OBJECT
    relative_humidity.cpp
    )

#
# Add this directory to the list of directories to look for include files
#
target_include_directories(humidity_units PRIVATE
  ${CMAKE_SOURCE_DIR}/src/lib
  ${CMAKE_CURRENT_SOURCE_DIR}
)

#
# Use the C++23 option
# Done higher up with add_compile_options
#target_compile_options(humidity_units PUBLIC -std=c++23)
