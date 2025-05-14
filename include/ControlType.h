#pragma once

typedef enum ControlType {
    CT_NONE,
#define CONTROL_TYPE(x, y) x,
#include "ControlType.inc"
#undef CONTROL_TYPE
    CT_MAX
} ControlType;
