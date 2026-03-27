#pragma once
// =============================================================================
//  clouds.h
//  Gently drifting clouds built from overlapping filled ellipses.
//
//  Each cloud has 4 puff ellipses arranged to form a natural cloud shape.
//  Clouds drift slowly to the right and wrap around the screen edge.
//  Cloud colour adapts to time of day (white at noon, warm pink at sunrise/set).
// =============================================================================

// Seed cloud positions and speeds. Call once at startup.
void initClouds();

// Move clouds rightward, wrapping at screen edge.
// dt : seconds since last frame
void updateClouds(float dt);

// Draw all clouds, tinted to match the current time of day.
// sunT : normalised sun progress [0, 1]
void drawClouds(float sunT);