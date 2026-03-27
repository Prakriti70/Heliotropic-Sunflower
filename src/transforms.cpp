#include "transforms.h"
// =============================================================================
//  transforms.cpp
//  Implementation of 2D transformation utilities.
// =============================================================================

#include <cmath>

static const float PI = 3.14159265358979f;

// ---------------------------------------------------------------------------
//  rotate2D
//  Standard pivot rotation in three steps:
//    1. Translate pivot to origin
//    2. Apply 2×2 rotation matrix
//    3. Translate back
// ---------------------------------------------------------------------------
glm::vec2 rotate2D(glm::vec2 p, glm::vec2 pivot, float theta) {
    float c = std::cos(theta);
    float s = std::sin(theta);

    // Translate so pivot is at origin
    p -= pivot;

    // Apply rotation matrix
    glm::vec2 rotated = {
        p.x * c - p.y * s,
        p.x * s + p.y * c
    };

    // Translate back
    return rotated + pivot;
}

// ---------------------------------------------------------------------------
//  lerpAngle
//  Wraps the angular difference into [-PI, PI] to ensure the shortest-path
//  rotation, then advances `from` by `t` of that difference.
// ---------------------------------------------------------------------------
float lerpAngle(float from, float to, float t) {
    float diff = to - from;

    // Normalise difference to shortest arc
    while (diff >  PI) diff -= 2.0f * PI;
    while (diff < -PI) diff += 2.0f * PI;

    return from + diff * t;
}

// ---------------------------------------------------------------------------
//  sunArcX / sunArcY
//  Maps t ∈ [0,1] to angle ∈ [PI, 0] (left to right along a semicircle).
//  angle(t) = PI*(1 - t)
//    t = 0   → angle = PI   → leftmost point  (sunrise)
//    t = 0.5 → angle = PI/2 → topmost point   (noon)
//    t = 1   → angle = 0    → rightmost point (sunset)
// ---------------------------------------------------------------------------
float sunArcX(float t, float cx, float /*cy*/, float r) {
    float angle = PI * (1.0f - t);
    return cx + r * std::cos(angle);
}

float sunArcY(float t, float /*cx*/, float cy, float r) {
    float angle = PI * (1.0f - t);
    return cy + r * std::sin(angle);   // sin > 0 for angle in (0, PI) → above centre
}