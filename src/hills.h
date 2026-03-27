#pragma once
// =============================================================================
//  hills.h
//  Three layers of rolling hills rendered back-to-front (painter's algorithm).
//
//  Each hill is a large filled ellipse whose centre sits below the horizon
//  so only the upper dome is visible.  The Midpoint Ellipse Algorithm is used
//  to draw a crisp ridge outline on top of each filled dome.
//
//  Layers (drawn in order):
//    1. Far hills  — blue-tinted, smaller, higher on screen
//    2. Mid hills  — medium green
//    3. Near hills — rich green, largest, just behind the ground strip
// =============================================================================

// Draw all three hill layers.
// sunT : used to tint hills with a golden-hour wash at sunrise/sunset.
void drawHills(float sunT);