#
# This file contains CPack settings. Cmake will build a
# default build/CPackConfig.cmake. At the end it includes a
# properties file. You can override those default properties
# with these settings here. It is important that the top
# level CmakeLists.txt file set the CPACK_PROPERTIES_FILE to
# this file prior to including CPack.
#
# The following page was used as a guide
# https://cmake.org/cmake/help/v3.7/module/CPackDeb.html
#
# Also:
# https://gitlab.kitware.com/cmake/community/-/wikis/doc/cpack/Configuration
# https://cmake.org/cmake/help/latest/cpack_gen/deb.html
#
# Debian specific settings
# 
set(CPACK_DEBIAN_PACKAGE_NAME "qw-units")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "chris@quietwind.net")
set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "arm64")
#
# We use libstdc++ in order to use the expect construct.
# We need libsystemd-dev in order to talk to systemd daemon via C++
# We need libfmt to format output messages
# We need libcurl4 in order to build a htps command to send to Weather Station
# We need libjsoncpp-dev to access the configuration json file via C++
# We need jq to access json configuration file for postinst
#
# packages needed for development
#set(DEVELOPMENT_LIBRARY_PACKAGES_NEEDED "")
set(RUNTIME_PACKAGES_NEEDED "libstdc++6 (>= 12.2.0-14)")

set(CPACK_DEBIAN_PACKAGE_DEPENDS ${RUNTIME_PACKAGES_NEEDED}, ${DEVELOPMENT_LIBRARY_PACKAGES_NEEDED})
set(CPACK_DEBIAN_PACKAGE_DESCRIPTION "Measuring units library")
set(CPACK_DEBIAN_PACKAGE_SECTION "embedded")
set(CPACK_DEBIAN_ARCHIVE_TYPE "gnutar")
set(CPACK_DEBIAN_COMPRESSION_TYPE "gzip")
set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")

#
# Non-DEBIAN settings
#
set(CPACK_BINARY_DEB "ON")
set(CPACK_GENERATOR "DEB")
set(CPACK_PACKAGE_VENDOR "QuietWind")

#
# Setup infor for package file name
#
set(CPACK_INNOSETUP_ARCHITECTURE ${CPACK_DEBIAN_PACKAGE_ARCHITECTURE})
set(CPACK_PACKAGE_VERSION_MAJOR "0")
set(CPACK_PACKAGE_VERSION_MINOR "1")
set(CPACK_PACKAGE_VERSION_PATCH "1")
set(CPACK_DEBIAN_PACKAGE_VERSION ${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH})
set(CPACK_DEBIAN_PACKAGE_RELEASE "2")
set(CPACK_PACKAGE_FILE_NAME "${CPACK_DEBIAN_PACKAGE_NAME}_${CPACK_DEBIAN_PACKAGE_VERSION}-${CPACK_DEBIAN_PACKAGE_RELEASE}_${CPACK_DEBIAN_PACKAGE_ARCHITECTURE}")

#
# Define the install prefix
#
set(CPACK_PACKAGING_INSTALL_PREFIX "/usr/local/qw")

#
# Define the postinst script and the pre removal script
# Apt wants these scripts named <package>.postinst and <package>.prerm.
# I didn't see this documented anywhere but got errors when I just had postinst and prerm.
# These files get installed in /var/lib/dpkg/info along with a package .list file and .md5sum file after installation
#
#set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA "${CMAKE_CURRENT_SOURCE_DIR}/src/config/postinst;${CMAKE_CURRENT_SOURCE_DIR}/src/config/prerm")
