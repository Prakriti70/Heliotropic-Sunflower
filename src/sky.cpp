#include "sky.h"
// =============================================================================
//  sky.cpp
//  Renders the sky as 40 horizontal gradient bands (filled quads).
//  Colours are linearly interpolated between zenith (top) and horizon (bottom)
//  at each of three key-frame moments, then blended between key-frames.
// =============================================================================

#include "renderer.h"
#include <glm/glm.hpp>
#include <cmath>

static const int BAND_COUNT = 40;   // more bands = smoother gradient

// ---------------------------------------------------------------------------
//  Colour key-frames
// ---------------------------------------------------------------------------
// Zenith (top of sky)
static const glm::vec4 ZEN_SUNRISE = { 0.32f, 0.12f, 0.42f, 1.0f };  // deep violet
static const glm::vec4 ZEN_NOON    = { 0.18f, 0.46f, 0.88f, 1.0f };  // bright blue
static const glm::vec4 ZEN_SUNSET  = { 0.12f, 0.08f, 0.30f, 1.0f };  // dark violet

// Horizon (bottom of sky, just above the hills)
static const glm::vec4 HOR_SUNRISE = { 1.00f, 0.55f, 0.22f, 1.0f };  // warm orange
static const glm::vec4 HOR_NOON    = { 0.58f, 0.80f, 1.00f, 1.0f };  // pale sky blue
static const glm::vec4 HOR_SUNSET  = { 1.00f, 0.35f, 0.08f, 1.0f };  // deep orange-red

// ---------------------------------------------------------------------------
//  drawSky
// ---------------------------------------------------------------------------
void drawSky(float sunT) {
    // Determine current zenith and horizon colours by interpolating key-frames
    glm::vec4 zenith, horizon;

    if (sunT <= 0.5f) {
        // Sunrise → Noon
        float t = sunT * 2.0f;                              // remap [0,0.5] → [0,1]
        zenith  = glm::mix(ZEN_SUNRISE, ZEN_NOON, t);
        horizon = glm::mix(HOR_SUNRISE, HOR_NOON, t);
    } else {
        // Noon → Sunset
        float t = (sunT - 0.5f) * 2.0f;                    // remap [0.5,1] → [0,1]
        zenith  = glm::mix(ZEN_NOON, ZEN_SUNSET, t);
        horizon = glm::mix(HOR_NOON, HOR_SUNSET, t);
    }

    // Draw horizontal bands from bottom (index 0 = horizon) to top (zenith)
    for (int i = 0; i < BAND_COUNT; ++i) {
        float y0   = (float)SCR_H * (float) i      / (float)BAND_COUNT;
        float y1   = (float)SCR_H * (float)(i + 1) / (float)BAND_COUNT;
        float frac = (float)i / (float)(BAND_COUNT - 1);   // 0=bottom, 1=top

        glm::vec4 bandColor = glm::mix(horizon, zenith, frac);

        Batch* b = newBatch(bandColor, GL_TRIANGLE_FAN);
        addPt(b, 0.0f,          y0);
        addPt(b, (float)SCR_W,  y0);
        addPt(b, (float)SCR_W,  y1);
        addPt(b, 0.0f,          y1);
    }
}