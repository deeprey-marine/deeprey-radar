# ~~~
# Summary:      Main plugin build script
# Copyright (c) 2020-2021 Mike Rossiter
# License:      GPLv3+
# ~~~

# This program is free software; you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free Software
# Foundation; either version 3 of the License, or (at your option) any later
# version.

# -------- Cmake setup ---------
#
cmake_minimum_required(VERSION 3.12.0)
include(${CMAKE_SOURCE_DIR}/cmake/CmakeSetup.cmake NO_POLICY_SCOPE)
message(STATUS "Cmake version: ${CMAKE_VERSION}.")

# -------- Build setup  --------
#
include(PluginOptions)
if(BUILD_TYPE)
  message(STATUS "Building: ${BUILD_TYPE}")
else()
  message(STATUS "Configuring")
endif()

# -------- Plugin setup ----------
#
include(Plugin.cmake)

# -------- Setup completed, build the plugin --------
#
project(${PKG_NAME} VERSION ${PKG_VERSION})
include(PluginCompiler)
if(NOT WIN32
   AND NOT "${CMAKE_SYSTEM_PROCESSOR}" MATCHES "arm"
   AND NOT "${CMAKE_SYSTEM_PROCESSOR}" MATCHES "aarch64")
  message(STATUS "CPU: ${CMAKE_SYSTEM_PROCESSOR}")
  add_compile_options(-msse4.2 -maes)
endif()

add_library(${CMAKE_PROJECT_NAME} SHARED EXCLUDE_FROM_ALL ${SRC} ${HEADERS})


if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC" AND DEPLOY_ON_BUILD)
    add_custom_command(
        TARGET ${CMAKE_PROJECT_NAME}
        POST_BUILD
        COMMAND copy "$(TargetPath)" "$(LOCALAPPDATA)\\opencpn\\plugins\\$(TargetFileName)"
        COMMENT "Deploy to import location"
        VERBATIM
    )
endif()

include_directories(
					BEFORE 
					${CMAKE_SOURCE_DIR}/include
					${CMAKE_SOURCE_DIR}/include/raymarine
					${CMAKE_SOURCE_DIR}/include/navico
					${CMAKE_SOURCE_DIR}/include/garminxhd
					${CMAKE_SOURCE_DIR}/include/garminhd
					${CMAKE_SOURCE_DIR}/include/emulator
)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/deeprey-api/radar"
					${CMAKE_SOURCE_DIR}/deeprey-api/radar/raymarine
					${CMAKE_SOURCE_DIR}/deeprey-api/radar/navico
					${CMAKE_SOURCE_DIR}/deeprey-api/radar/garminxhd
					${CMAKE_SOURCE_DIR}/deeprey-api/radar/garminhd
					${CMAKE_SOURCE_DIR}/deeprey-api/radar/emulator
)


add_subdirectory("opencpn-libs/${PKG_API_LIB}")
if(${PKG_NAME} MATCHES wx32)
  target_link_libraries(${CMAKE_PROJECT_NAME} ocpn::api_wx32)
else()
  target_link_libraries(${CMAKE_PROJECT_NAME} ocpn::api)
endif()

set(VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(VERSION_PATCH ${PROJECT_VERSION_PATCH})
if(PROJECT_VERSION_TWEAK STREQUAL "")
  set(PROJECT_VERSION_TWEAK 0)
endif()

set(PACKAGE_NAME ${CMAKE_PROJECT_NAME})

# Create the minimim ocpn version id string like ov50
string(REGEX REPLACE "\([0-9]\)\.\([0-9]\).*" "ov\\1\\2" OCPN_MIN_VERSION
                     ${API_OCPN_MIN_VERSION})

if(COMMAND late_init)
  late_init()
endif()

# Configure the flatpak manifest
file(GLOB manifest
     ${PROJECT_SOURCE_DIR}/flatpak/org.opencpn.OpenCPN.Plugin.*.yaml)
include(ConfigureManifest)
configure_manifest(${manifest} _new_manifest)

# Set up targets. Targets sets up new targets with BUILD_TYPE set to 'flatpak',
# 'android' or 'tarball'. The initial call without BUILD_TYPE ends here.
#
include(Targets)
create_targets(${_new_manifest})

if("${BUILD_TYPE}" STREQUAL "")
  return()
endif()

if(NOT ${BUILD_TYPE} STREQUAL "flatpak")
  # Build package as required (flatpak already dealt with).
  #
  if(APPLE)
    include(MacosWxwidgets)
  endif()
  include(PluginInstall)
  if(QT_ANDROID)
    include(AndroidLibs)
  else()
    include(PluginLibs)
  endif()
  include(PluginLocalization)

  include_directories(BEFORE ${CMAKE_BINARY_DIR}/include)

  if(COMMAND add_plugin_libraries)
    add_plugin_libraries()
  endif()

endif()

configure_file(${CMAKE_SOURCE_DIR}/config.h.in
               ${CMAKE_BINARY_DIR}/include//config.h @ONLY)

configure_file(
  # The cloudsmith upload script
  ${CMAKE_SOURCE_DIR}/ci/upload.sh.in ${CMAKE_BINARY_DIR}/upload.sh @ONLY)
configure_file(
  # The cloudsmith upload script, windows bat file.
  ${CMAKE_SOURCE_DIR}/ci/upload.bat.in ${CMAKE_BINARY_DIR}/upload.bat @ONLY)

set(FILE_CHECKSUM "@checksum@")
configure_file(
  # The XML metadata file
  ${CMAKE_SOURCE_DIR}/plugin.xml.in
  ${CMAKE_BINARY_DIR}/${pkg_displayname}.xml.in @ONLY)
