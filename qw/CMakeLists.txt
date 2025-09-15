add_subdirectory(units)
add_subdirectory(logger)

# The devices are specific to Raspberry Pi
# so if it is not a arm64 build don't add them
# I did look it up and it seems that some
# x86_64 devices can have i2c devices and gpio
# devices. It also states that they should be
# compatible. But since I don;t have one to try
# I will just limit this to arm64 builds
if(CMAKE_SYSTEM_PROCESSOR STREQUAL "arm64")
  add_subdirectory(devices)
endif()

