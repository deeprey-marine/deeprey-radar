#pragma once


//
// The order of these is used for deciding what the 'best' representation is
// of the radar icon in the OpenCPN toolbar. The 'highest' (last) in the enum
// across all radar states is what is shown.
// If you add a RadarState also add an entry to g_toolbarIconColor...
//
enum RadarState {
    RADAR_OFF,
    RADAR_STANDBY,
    RADAR_WARMING_UP,
    RADAR_TIMED_IDLE,
    RADAR_STOPPING,
    RADAR_SPINNING_DOWN,
    RADAR_STARTING,
    RADAR_SPINNING_UP,
    RADAR_TRANSMIT
};

