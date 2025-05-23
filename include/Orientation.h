#pragma once

// Orientation HEAD_UP is available if there is no heading or dev mode is
// switched on Other orientations are available if there is a heading
#define ORIENTATION_HEAD_UP                                                    \
    (0) // Unstabilized heading (as if without compass) // Available if no
        // compass or in dev mode
#define ORIENTATION_STABILIZED_UP                                              \
    (1) // Stabilized heading (averaged over a few seconds)
#define ORIENTATION_NORTH_UP (2) // North up
#define ORIENTATION_COG_UP (3) // Averaged GPS COG up (same way as OpenCPN)
#define ORIENTATION_NUMBER (4)
