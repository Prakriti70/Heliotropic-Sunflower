#pragma once
// =============================================================================
//  sun.h
//  Animated sun: moves along a parametric semicircular arc across the sky.
//
//  Drawing:
//    - 5 concentric glow rings  (Midpoint Circle Algorithm, decreasing alpha)
//    - Solid warm-yellow disc   (filled circle — triangle fan)
//    - Small bright core highlight
//
//  Motion:
//    sunT ∈ [0,1] drives x(t) = cx + r*cos(PI*(1-t))
//                               y(t) = cy + r*sin(PI*(1-t))
//    Speed is adjustable; sunT loops back to 0 after reaching 1.
// =============================================================================

#include <glm/glm.hpp>

// Advance the sun's position along the arc.
// dt    : seconds since last frame
// speed : arc units per second  (try 0.04 for a ~25-second day)
void updateSun(float dt, float speed);

// Draw the sun (glow + disc) at its current position.
void drawSun();

// Return the current sun centre in pixel space.
glm::vec2 getSunPos();

// Return normalised progress t ∈ [0, 1].
float getSunT();

// Reset to sunrise position.
void resetSun();