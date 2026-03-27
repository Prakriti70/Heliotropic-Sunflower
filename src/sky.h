#pragma once
// =============================================================================
//  sky.h
//  Animated sky gradient background.
//
//  The sky colour shifts through three phases driven by sunT:
//    Sunrise (t≈0.0) : violet zenith → warm orange horizon
//    Noon    (t≈0.5) : bright blue zenith → pale sky-blue horizon
//    Sunset  (t≈1.0) : dark violet zenith → deep orange-red horizon
//
//  Drawn as BAND_COUNT thin horizontal filled quads blended top-to-bottom.
// =============================================================================

// Draw the sky gradient.
// sunT : normalised sun progress in [0, 1]  (0=sunrise, 0.5=noon, 1=sunset)
void drawSky(float sunT);