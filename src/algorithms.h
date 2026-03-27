#pragma once
// =============================================================================
//  algorithms.h
//  All fundamental raster drawing algorithms required by the project proposal.
//
//  Every function appends pixel vertices into a Batch (GL_POINTS or
//  GL_TRIANGLE_FAN) so they are rendered by the batch renderer in renderer.cpp.
//
//  Algorithms implemented:
//    1. DDA Line               (floating-point incremental)
//    2. Bresenham's Line       (integer arithmetic only)
//    3. Midpoint Circle        (8-fold symmetry)
//    4. Midpoint Ellipse       (4-fold symmetry, two regions)
//
//  Higher-level filled helpers (using triangle fans):
//    5. filledCircle           (for discs, sun body)
//    6. filledEllipse          (for hills, clouds, petals, leaves)
// =============================================================================

#include "renderer.h"

// ---------------------------------------------------------------------------
//  1. DDA Line Algorithm
//     steps = max(|dx|, |dy|),  xi = dx/steps,  yi = dy/steps
//     Plots round(x), round(y) at each step.
//     Good for: stems, angled lines requiring sub-pixel smoothness.
// ---------------------------------------------------------------------------
void ddaLine(Batch* b, float x1, float y1, float x2, float y2);

// ---------------------------------------------------------------------------
//  2. Bresenham's Line Algorithm
//     Integer-only arithmetic. Uses an error term to decide whether to
//     step in x, y, or both at each pixel.
//     Good for: structural outlines, grass blades, leaf veins.
// ---------------------------------------------------------------------------
void bresenhamLine(Batch* b, int x1, int y1, int x2, int y2);

// ---------------------------------------------------------------------------
//  3. Midpoint Circle Algorithm
//     Exploits 8-fold symmetry — only the first octant is computed.
//     Decision parameter p = 1 - r initially.
//     Good for: sun outline, disc outline, glow rings.
// ---------------------------------------------------------------------------
void midpointCircle(Batch* b, int cx, int cy, int r);

// ---------------------------------------------------------------------------
//  4. Midpoint Ellipse Algorithm
//     Two-region approach to minimise rounding error.
//     Region 1: while 2*ry²*x < 2*rx²*y  (gentle slope)
//     Region 2: while y > 0               (steep slope)
//     Good for: hill domes, cloud puffs, petal/leaf outlines.
// ---------------------------------------------------------------------------
void midpointEllipse(Batch* b, int cx, int cy, int rx, int ry);

// ---------------------------------------------------------------------------
//  5. Filled Circle  (triangle fan)
//     Approximates a filled disc with `segments` triangles.
//     Good for: sun body, flower disc, seed dots.
// ---------------------------------------------------------------------------
void filledCircle(Batch* b, float cx, float cy, float r, int segments = 60);

// ---------------------------------------------------------------------------
//  6. Filled Ellipse  (triangle fan)
//     Approximates a filled ellipse with `segments` triangles.
//     Good for: hills, cloud puffs, petals, leaves, shadow.
// ---------------------------------------------------------------------------
void filledEllipse(Batch* b, float cx, float cy,
                   float rx, float ry, int segments = 60);