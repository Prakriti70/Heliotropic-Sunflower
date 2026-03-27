#include "algorithms.h"
// =============================================================================
//  algorithms.cpp
//  Implementations of all four required raster algorithms plus filled helpers.
//
//  Reference: Hearn D., Baker M.P. — "Computer Graphics C Version", 2nd ed.
//             Chapters 3.2 (DDA), 3.3 (Bresenham), 3.4 (Circle), 3.5 (Ellipse)
// =============================================================================

#include <cmath>
#include <algorithm>

static const float PI = 3.14159265358979f;

// =============================================================================
//  1. DDA LINE ALGORITHM
//
//  Key idea: divide the line into 'steps' equal increments.
//    steps = max(|dx|, |dy|)   ensures no gaps between plotted pixels.
//    xi    = dx / steps        x increment per step
//    yi    = dy / steps        y increment per step
//  Each step: plot (round(x), round(y)), then x+=xi, y+=yi.
// =============================================================================
void ddaLine(Batch* b, float x1, float y1, float x2, float y2) {
    float dx    = x2 - x1;
    float dy    = y2 - y1;
    float steps = std::max(std::abs(dx), std::abs(dy));

    // Degenerate case: single point
    if (steps == 0.0f) {
        addPt(b, x1, y1);
        return;
    }

    float xi = dx / steps;  // x increment per step
    float yi = dy / steps;  // y increment per step
    float x  = x1;
    float y  = y1;

    for (int i = 0; i <= (int)steps; ++i) {
        addPt(b, std::round(x), std::round(y));
        x += xi;
        y += yi;
    }
}

// =============================================================================
//  2. BRESENHAM'S LINE ALGORITHM
//
//  Key idea: maintain an integer error term to track the ideal line.
//    err = dx - dy   (initial)
//    Each step:
//      if 2*err > -dy  → step in x, err -= dy
//      if 2*err <  dx  → step in y, err += dx
//  Advantage: no floating-point arithmetic at all.
// =============================================================================
void bresenhamLine(Batch* b, int x1, int y1, int x2, int y2) {
    int dx =  std::abs(x2 - x1);
    int dy =  std::abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;   // x step direction
    int sy = (y1 < y2) ? 1 : -1;   // y step direction
    int err = dx - dy;

    while (true) {
        addPt(b, (float)x1, (float)y1);

        if (x1 == x2 && y1 == y2) break;   // reached end point

        int e2 = 2 * err;
        if (e2 > -dy) { err -= dy; x1 += sx; }  // step x
        if (e2 <  dx) { err += dx; y1 += sy; }  // step y
    }
}

// =============================================================================
//  3. MIDPOINT CIRCLE ALGORITHM
//
//  Key idea: start at (0, r) and step clockwise through the first octant.
//  Decision parameter p = 1 - r:
//    p < 0  → next point is (x+1,  y  ),  p_new = p + 2x + 3
//    p >= 0 → next point is (x+1, y-1),   p_new = p + 2x - 2y + 5
//  8-fold symmetry: one computed point → 8 reflected points.
// =============================================================================
void midpointCircle(Batch* b, int cx, int cy, int r) {
    int x = 0;
    int y = r;
    int p = 1 - r;   // initial decision parameter

    // Plot all 8 symmetric points for a given (x, y) in the first octant
    auto plot8 = [&](int px, int py) {
        addPt(b, (float)(cx + px), (float)(cy + py));
        addPt(b, (float)(cx - px), (float)(cy + py));
        addPt(b, (float)(cx + px), (float)(cy - py));
        addPt(b, (float)(cx - px), (float)(cy - py));
        addPt(b, (float)(cx + py), (float)(cy + px));
        addPt(b, (float)(cx - py), (float)(cy + px));
        addPt(b, (float)(cx + py), (float)(cy - px));
        addPt(b, (float)(cx - py), (float)(cy - px));
    };

    plot8(x, y);
    while (x < y) {
        ++x;
        if (p < 0) {
            p += 2 * x + 1;           // midpoint is inside circle
        } else {
            --y;
            p += 2 * (x - y) + 1;    // midpoint is outside circle
        }
        plot8(x, y);
    }
}

// =============================================================================
//  4. MIDPOINT ELLIPSE ALGORITHM
//
//  Split into two regions based on the gradient of the ellipse boundary:
//    Region 1: |dy/dx| < 1  →  start at (0, ry), step right while px < py
//    Region 2: |dy/dx| > 1  →  continue until y = 0
//  Decision parameters p1, p2 updated incrementally using integer arithmetic.
//  4-fold symmetry: one computed point → 4 reflected points.
// =============================================================================
void midpointEllipse(Batch* b, int cx, int cy, int rx, int ry) {
    long long rx2    = (long long)rx * rx;
    long long ry2    = (long long)ry * ry;
    long long twoRx2 = 2LL * rx2;
    long long twoRy2 = 2LL * ry2;

    // Plot 4 symmetric points for a given (x,y) offset from centre
    auto plot4 = [&](int px, int py) {
        addPt(b, (float)(cx + px), (float)(cy + py));
        addPt(b, (float)(cx - px), (float)(cy + py));
        addPt(b, (float)(cx + px), (float)(cy - py));
        addPt(b, (float)(cx - px), (float)(cy - py));
    };

    int x = 0, y = ry;
    long long px = 0, py = twoRx2 * (long long)y;

    // --- Region 1: step in x direction ------------------------------------
    long long p1 = (long long)std::round(
        (double)ry2 - (double)rx2 * ry + 0.25 * (double)rx2);
    plot4(x, y);

    while (px < py) {
        ++x;
        px += twoRy2;
        if (p1 < 0) {
            p1 += ry2 + px;
        } else {
            --y;
            py -= twoRx2;
            p1 += ry2 + px - py;
        }
        plot4(x, y);
    }

    // --- Region 2: step in y direction ------------------------------------
    long long p2 = (long long)std::round(
        (double)ry2 * ((double)x + 0.5) * ((double)x + 0.5) +
        (double)rx2 * ((double)(y - 1)) * ((double)(y - 1)) -
        (double)rx2 * (double)ry2);

    while (y > 0) {
        --y;
        py -= twoRx2;
        if (p2 > 0) {
            p2 += rx2 - py;
        } else {
            ++x;
            px += twoRy2;
            p2 += rx2 - py + px;
        }
        plot4(x, y);
    }
}

// =============================================================================
//  5. FILLED CIRCLE  (triangle fan approximation)
//
//  Centre vertex followed by `segments+1` perimeter vertices.
//  GL_TRIANGLE_FAN connects them as a filled disc.
// =============================================================================
void filledCircle(Batch* b, float cx, float cy, float r, int segments) {
    addPt(b, cx, cy);   // fan centre
    for (int i = 0; i <= segments; ++i) {
        float angle = 2.0f * PI * (float)i / (float)segments;
        addPt(b, cx + r * std::cos(angle),
                 cy + r * std::sin(angle));
    }
}

// =============================================================================
//  6. FILLED ELLIPSE  (triangle fan approximation)
// =============================================================================
void filledEllipse(Batch* b, float cx, float cy,
                   float rx, float ry, int segments) {
    addPt(b, cx, cy);   // fan centre
    for (int i = 0; i <= segments; ++i) {
        float angle = 2.0f * PI * (float)i / (float)segments;
        addPt(b, cx + rx * std::cos(angle),
                 cy + ry * std::sin(angle));
    }
}