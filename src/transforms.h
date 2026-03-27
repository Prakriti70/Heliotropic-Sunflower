#pragma once
// =============================================================================
//  transforms.h
//  2D geometric transformation helpers used by all scene modules.
//
//  All functions operate in pixel space on glm::vec2 points.
//
//  The 2D rotation matrix (from syllabus Chapter 3):
//      x' = (x - cx)*cosθ - (y - cy)*sinθ + cx
//      y' = (x - cx)*sinθ + (y - cy)*cosθ + cy
// =============================================================================

#include <glm/glm.hpp>

// ---------------------------------------------------------------------------
//  rotate2D
//  Rotate point `p` around `pivot` by `theta` radians (counter-clockwise).
//  Implements the standard 2D rotation matrix from the project proposal.
// ---------------------------------------------------------------------------
glm::vec2 rotate2D(glm::vec2 p, glm::vec2 pivot, float theta);

// ---------------------------------------------------------------------------
//  lerpAngle
//  Smoothly interpolate from angle `from` toward angle `to` by fraction `t`.
//  Handles wrap-around so the flower never spins the "wrong way".
//  t = 0 → returns `from`,  t = 1 → returns `to`.
// ---------------------------------------------------------------------------
float lerpAngle(float from, float to, float t);

// ---------------------------------------------------------------------------
//  sunArcX / sunArcY
//  Parametric semicircular arc for the sun's path across the sky.
//  t ∈ [0, 1]:  0 = sunrise (left),  0.5 = zenith (top),  1 = sunset (right)
//  cx, cy : arc centre in pixel space    r : arc radius
// ---------------------------------------------------------------------------
float sunArcX(float t, float cx, float cy, float r);
float sunArcY(float t, float cx, float cy, float r);