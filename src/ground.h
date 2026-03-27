#pragma once
// =============================================================================
//  ground.h
//  Flat green ground strip at the bottom of the scene.
//
//  Components:
//    1. Solid filled rectangle  (ground base colour)
//    2. Grass blades along the top edge — drawn using Bresenham's Line
//       Algorithm.  Each blade is a pair of short diagonal lines forming
//       a natural V-shape, spaced every 8 pixels across the screen width.
// =============================================================================

// Draw the ground strip and grass edge.
// sunT : time-of-day value [0,1] used for golden-hour colour tinting.
void drawGround(float sunT);