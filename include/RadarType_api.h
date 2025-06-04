#pragma once

namespace RadarPlugin {

typedef enum RadarType {
#define DEFINE_RADAR(t, n, s, l, a, b, c, d) t,
#include "RadarType.h"
    RT_MAX
} RadarType;

}
