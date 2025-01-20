cmake_minimum_required(VERSION 3.10)

project(radar_pi)

# Active l’affichage des dossiers dans Visual Studio
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

#
# -------- Options / Variables du plugin --------
#
set(OCPN_TEST_REPO
    "opencpn-radar-pi/radar-unstable"
    CACHE STRING "Default repository for untagged builds"
)
set(OCPN_BETA_REPO
    "opencpn-radar-pi/radar-beta"
    CACHE STRING 
    "Default repository for tagged builds matching 'beta'"
)
set(OCPN_RELEASE_REPO
    "opencpn-radar-pi/radar-prod"
    CACHE STRING 
    "Default repository for tagged builds not matching 'beta'"
)

set(PKG_NAME       radar_pi)
set(PKG_VERSION    5.5.5)
set(PKG_PRERELEASE "")  # vide ou p. ex "beta"

set(DISPLAY_NAME    "radar")
set(PLUGIN_API_NAME "Radar")

set(CPACK_PACKAGE_CONTACT "kees@verruijt.net")
set(PKG_SUMMARY     "Overlays the radar picture on OpenCPN")
set(PKG_DESCRIPTION [=[
Garmin, Navico et Raymarine radar support

WARNING: OPENGL MODE IS REQUIRED!

Works with Garmin HD, xHD, Navico BR24, 3G, 4G, HALOxx and several Raymarine radars.

When a compass heading is provided it will allow radar overlay on the chart(s).
It also allows separate display of a traditional radar picture (PPI).

Supports (M)ARPA, Guard zones, AIS overlay on PPI, etc.
]=])
set(PKG_AUTHOR        "Hakan Svensson / Douwe Fokkema / Kees Verruijt / David S Register")
set(PKG_IS_OPEN_SOURCE "yes")
set(PKG_HOMEPAGE       "https://github.com/opencpn-radar-pi/radar_pi")
set(PKG_INFO_URL       "https://opencpn.org/OpenCPN/plugins/radarPI.html")

#
# -------- Fichiers par dossier / composante --------
#

# 1) Général
set(GENERAL_HEADERS
    include/ControlsDialog.h
    include/GuardZone.h
    include/GuardZoneBogey.h
    include/Kalman.h
    include/Matrix.h
    include/MessageBox.h
    include/OptionsDialog.h
#    include/RadarCanvas.h
    include/RadarControl.h
    include/RadarControlItem.h
    include/RadarDraw.h
    include/RadarDrawShader.h
    include/RadarDrawVertex.h
    include/RadarFactory.h
#    include/RadarInfo.h
    include/RadarLocationInfo.h
    include/Arpa.h
#    include/RadarPanel.h
    include/RadarReceive.h
    include/RadarType.h
    include/SelectDialog.h
    include/SoftwareControlSet.h
    include/TextureFont.h
    include/TrailBuffer.h
    include/drawutil.h
    include/icons.h
    include/pi_common.h
#    include/radar_pi.h
    include/shaderutil.h
    include/socketutil.h
#    include/RadarAPI.h
#   include/DpRadarCommand.h
)

set(GENERAL_SOURCES
    src/ControlsDialog.cpp
    src/GuardZone.cpp
    src/GuardZoneBogey.cpp
    src/Kalman.cpp
    src/MessageBox.cpp
    src/OptionsDialog.cpp
#    src/RadarCanvas.cpp
    src/RadarDraw.cpp
    src/RadarDrawShader.cpp
    src/RadarDrawVertex.cpp
    src/RadarFactory.cpp
#    src/RadarInfo.cpp
    src/Arpa.cpp
#    src/RadarPanel.cpp
    src/SelectDialog.cpp
    src/TextureFont.cpp
    src/TrailBuffer.cpp
    src/drawutil.cpp
    src/icons.cpp
#    src/radar_pi.cpp
    src/shaderutil.cpp
    src/socketutil.cpp
#    src/RadarAPI.cpp
#    src/DpRadarCommand.cpp
)

# Fichiers .inc
set(INLINE_INCLUDES
    include/ControlType.inc
    include/shaderutil.inc
)

# 2) Emulator
set(EMULATOR_HEADERS
    include/emulator/EmulatorControl.h
    include/emulator/EmulatorControlSet.h
    include/emulator/EmulatorControlsDialog.h
    include/emulator/EmulatorReceive.h
    include/emulator/emulatortype.h
)
set(EMULATOR_SOURCES
    src/emulator/EmulatorControl.cpp
    src/emulator/EmulatorControlsDialog.cpp
    src/emulator/EmulatorReceive.cpp
)

# 3) GarminHD
set(GARMINHD_HEADERS
    include/garminhd/GarminHDControl.h
    include/garminhd/GarminHDControlSet.h
    include/garminhd/GarminHDControlsDialog.h
    include/garminhd/GarminHDReceive.h
    include/garminhd/garminhdtype.h
)
set(GARMINHD_SOURCES
    src/garminhd/GarminHDControl.cpp
    src/garminhd/GarminHDControlsDialog.cpp
    src/garminhd/GarminHDReceive.cpp
)

# 4) GarminxHD
set(GARMINXHD_HEADERS
    include/garminxhd/GarminxHDControl.h
    include/garminxhd/GarminxHDControlSet.h
    include/garminxhd/GarminxHDControlsDialog.h
    include/garminxhd/GarminxHDReceive.h
    include/garminxhd/garminxhdtype.h
)
set(GARMINXHD_SOURCES
    src/garminxhd/GarminxHDControl.cpp
    src/garminxhd/GarminxHDControlsDialog.cpp
    src/garminxhd/GarminxHDReceive.cpp
)

# 5) Navico
set(NAVICO_HEADERS
    include/navico/Header.h
    include/navico/NavicoCommon.h
    include/navico/NavicoControl.h
    include/navico/NavicoControlSet.h
    include/navico/NavicoControlsDialog.h
    include/navico/NavicoLocate.h
    include/navico/NavicoReceive.h
    include/navico/br24type.h
    include/navico/br3gtype.h
    include/navico/br4gatype.h
    include/navico/br4gbtype.h
    include/navico/haloatype.h
    include/navico/halobtype.h
)
set(NAVICO_SOURCES
    src/navico/NavicoControl.cpp
    src/navico/NavicoControlsDialog.cpp
    src/navico/NavicoLocate.cpp
    src/navico/NavicoReceive.cpp
)

# 6) Raymarine
set(RAYMARINE_HEADERS
    include/raymarine/RME120Control.h
    include/raymarine/RMQuantumControl.h
    include/raymarine/RME120ControlSet.h
    include/raymarine/RME120ControlsDialog.h
    include/raymarine/RaymarineReceive.h
    include/raymarine/RME120type.h
    include/raymarine/RMQuantumtype.h
    include/raymarine/RaymarineCommon.h
    include/raymarine/RaymarineLocate.h
    include/raymarine/RMQuantumControlsDialog.h
    include/raymarine/RMQuantumControlSet.h
)
set(RAYMARINE_SOURCES
    src/raymarine/RME120Control.cpp
    src/raymarine/RMQuantumControl.cpp
    src/raymarine/RME120ControlsDialog.cpp
    src/raymarine/RaymarineReceive.cpp
    src/raymarine/RaymarineLocate.cpp
    src/raymarine/RMQuantumControlsDialog.cpp
)


#
# === 1) Définir la liste de vos fichiers "importants" ===
#
set(IMPORTANT_HEADERS
    include/RadarPanel.h
    include/radar_pi.h
    include/RadarInfo.h
    include/RadarCanvas.h
    include/RadarAPI.h
    include/DpRadarCommand.h
)

set(IMPORTANT_SOURCES
    src/RadarPanel.cpp
    src/radar_pi.cpp
    src/RadarInfo.cpp
    src/RadarCanvas.cpp
    src/RadarAPI.cpp
    src/DpRadarCommand.cpp
)

#
# -------- Groups (filtres) Visual Studio --------
#

# Général
source_group("General\\Headers" FILES ${GENERAL_HEADERS})
source_group("General\\Sources" FILES ${GENERAL_SOURCES})
source_group("General\\Inline"  FILES ${INLINE_INCLUDES})

# Important
source_group("Important\\Headers" FILES ${IMPORTANT_HEADERS})
source_group("Important\\Sources" FILES ${IMPORTANT_SOURCES})

# Emulator
source_group("Emulator\\Headers" FILES ${EMULATOR_HEADERS})
source_group("Emulator\\Sources" FILES ${EMULATOR_SOURCES})

# GarminHD
source_group("GarminHD\\Headers" FILES ${GARMINHD_HEADERS})
source_group("GarminHD\\Sources" FILES ${GARMINHD_SOURCES})

# GarminxHD
source_group("GarminxHD\\Headers" FILES ${GARMINXHD_HEADERS})
source_group("GarminxHD\\Sources" FILES ${GARMINXHD_SOURCES})

# Navico
source_group("Navico\\Headers" FILES ${NAVICO_HEADERS})
source_group("Navico\\Sources" FILES ${NAVICO_SOURCES})

# Raymarine
source_group("Raymarine\\Headers" FILES ${RAYMARINE_HEADERS})
source_group("Raymarine\\Sources" FILES ${RAYMARINE_SOURCES})



#
# -------- Listes globales (pour usage en amont) --------
#
set(SRC
    ${GENERAL_SOURCES}
    ${IMPORTANT_SOURCES}
    ${EMULATOR_SOURCES}
    ${GARMINHD_SOURCES}
    ${GARMINXHD_SOURCES}
    ${NAVICO_SOURCES}
    ${RAYMARINE_SOURCES}
)

set(HEADERS
    ${GENERAL_HEADERS}
    ${IMPORTANT_HEADERS}
    ${EMULATOR_HEADERS}
    ${GARMINHD_HEADERS}
    ${GARMINXHD_HEADERS}
    ${NAVICO_HEADERS}
    ${RAYMARINE_HEADERS}
)

# Si vous voulez aussi voir .inc apparaître
set(INLINES
    ${INLINE_INCLUDES}
)

# Forcer l’apparition des .cpp et .h (et .inc) à la racine 
# quand on ne se trouve pas déjà dans un autre groupe
source_group("" FILES ${SRC} ${HEADERS} ${INLINES})



set(PKG_API_LIB api-19)  # ex. : un répertoire dans libs/ => api-16 ou api-17

#
# -------- Macros complémentaires --------
#
macro(late_init)
  # Exécute des initialisations après la création du plugin
  # Par exemple : corriger les avertissements OpenGL sur Mac
  target_compile_definitions(${PKG_NAME} PRIVATE GL_SILENCE_DEPRECATION)

  target_include_directories(${PKG_NAME} PUBLIC
      ${CMAKE_CURRENT_LIST_DIR}/include
      ${CMAKE_CURRENT_LIST_DIR}/include/emulator
      ${CMAKE_CURRENT_LIST_DIR}/include/garminhd
      ${CMAKE_CURRENT_LIST_DIR}/include/garminxhd
      ${CMAKE_CURRENT_LIST_DIR}/include/navico
      ${CMAKE_CURRENT_LIST_DIR}/include/raymarine
  )
endmacro()

macro(add_plugin_libraries)
  # Add libraries required by this plugin
  add_subdirectory("${CMAKE_SOURCE_DIR}/opencpn-libs/plugin_dc")
  target_link_libraries(${PACKAGE_NAME} ocpn::plugin-dc)
  
  # Ajoute ici les librairies requises par votre plugin
  add_subdirectory("opencpn-libs/nmea0183")
  target_link_libraries(${PKG_NAME} ocpn::nmea0183)

  add_subdirectory("opencpn-libs/wxJSON")
  target_link_libraries(${PKG_NAME} ocpn::wxjson)
endmacro()
