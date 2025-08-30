add_library(uvi_units OBJECT
    uvi.cpp
    )

#
# Add this directory to the list of directories to look for include files
#
target_include_directories(uvi_units PRIVATE
  ${CMAKE_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}
)

#
# Use the C++23 option
# Set higher up using add_complier_options
#target_compile_options(uvi_units PUBLIC -std=c++23)
