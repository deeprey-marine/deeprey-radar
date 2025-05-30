#pragma once

#include <wx/wx.h>
#define DEFINE_RADAR(t, n, s, l, a, b, c, d)
#include "RadarType.h"

typedef int SpokeBearing; // A value from 0 -- LINES_PER_ROTATION indicating a
                          // bearing (? = North, +ve = clockwise)

typedef int AngleDegrees; // An angle relative to North or HeadUp. Generally
                          // [0..359> or [-180,180]

typedef enum GuardZoneType { GZ_ARC, GZ_CIRCLE } GuardZoneType;

#define MOD_DEGREES_180(angle) (((int)(angle) + 900) % 360 - 180)
#define DEGREES_PER_ROTATION (360) // Classical math
#define MOD_DEGREES(angle)                                                     \
    (((angle) + 2 * DEGREES_PER_ROTATION) % DEGREES_PER_ROTATION)

