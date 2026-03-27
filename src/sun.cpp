#include "sun.h"
// =============================================================================
//  sun.cpp
//  Sun rises from behind the right hill with warm light rays.
//
//  The arc is adjusted so the sun starts low-right (behind the right hill)
//  at sunrise (sunT=0), crests overhead at noon (sunT=0.5), and sets
//  low-left at sunset (sunT=1).
//
//  Syllabus coverage:
//    - Chapter 7 : Key-frame animation (arc parametric motion)
//    - Chapter 2 : Midpoint Circle Algorithm for glow rings
//    - Chapter 6 : Illumination — ambient glow, light rays simulation
// =============================================================================

#include "renderer.h"
#include "algorithms.h"
#include "transforms.h"
#include <cmath>

static const float PI = 3.14159265358979f;

// ---------------------------------------------------------------------------
//  Arc parameters
//  Sun starts at bottom-right (behind the right hill peak ~x=680, y=310)
//  and arcs overhead, ending bottom-left.
// ---------------------------------------------------------------------------
static const float ARC_CX = 450.0f;   // arc horizontal centre
static const float ARC_CY = 320.0f;   // raised center so endpoints are above ground
static const float ARC_RX = 400.0f;   // wide — sun rises far left, sets far right
static const float ARC_RY = 280.0f;   // vertical — peaks near top of screen
static const float ARC_R  = 280.0f;   // kept for heightFactor

static const float SUN_R  = 32.0f;    // solid disc radius

// Arc goes from angle PI (left) to 0 (right) — left=sunset, right=sunrise
// sunT=0 → right side (sunrise behind right hill)
// sunT=1 → left side (sunset)

static float g_sunT = 0.0f;
static float g_sunX = 0.0f;
static float g_sunY = 0.0f;

// ---------------------------------------------------------------------------
//  updateSun
// ---------------------------------------------------------------------------
void updateSun(float dt, float speed) {
    g_sunT += dt * speed;
    if (g_sunT > 1.0f) g_sunT = 0.0f;

    // Elliptical arc: sun rises from center-left, peaks at top, sets center-right
    // x uses smaller radius (ARC_RX) → stays near center horizontally
    // y uses larger radius (ARC_RY) → rises high at noon
    float angle = PI * g_sunT;
    g_sunX = ARC_CX + ARC_RX * std::cos(PI - angle);  // far left to far right
    g_sunY = ARC_CY + ARC_RY * std::sin(PI - angle);  // above ground at all times
}

// ---------------------------------------------------------------------------
//  drawSun
// ---------------------------------------------------------------------------
void drawSun() {
    float cx = g_sunX;
    float cy = g_sunY;

    // How close to horizon (0=horizon, 1=zenith) — used for ray intensity
    float heightFactor = (g_sunY - ARC_CY) / ARC_RY;
    heightFactor = glm::clamp(heightFactor, 0.0f, 1.0f);

    // Near horizon: more orange glow; high up: more white-yellow
    float warmth = 1.0f - heightFactor * 0.6f;

    // -----------------------------------------------------------------------
    //  Horizon glow — wide soft ellipse when sun is low (sunrise/sunset)
    // -----------------------------------------------------------------------
    if (heightFactor < 0.35f) {
        float glowStrength = (0.35f - heightFactor) / 0.35f;
        glm::vec4 glowCol = {
            1.0f,
            0.55f + warmth * 0.25f,
            0.10f,
            0.12f * glowStrength
        };
        // Draw several expanding ellipses for a soft bloom
        for (int i = 4; i >= 1; --i) {
            Batch* hg = newBatch(
                { glowCol.r, glowCol.g, glowCol.b, glowCol.a * i * 0.5f },
                GL_TRIANGLE_FAN);
            filledEllipse(hg, cx, cy, SUN_R * (2.5f + i * 2.0f),
                          SUN_R * (1.0f + i * 0.8f));
        }
    }

    // -----------------------------------------------------------------------
    //  Glow rings (Midpoint Circle Algorithm) — 6 concentric rings
    // -----------------------------------------------------------------------
    for (int i = 6; i >= 1; --i) {
        float alpha  = 0.05f * (float)i * (0.5f + warmth * 0.5f);
        int   radius = (int)SUN_R + i * 13;
        glm::vec4 ringCol = {
            1.0f,
            0.85f + (1.0f - warmth) * 0.10f,
            0.30f + heightFactor * 0.40f,
            alpha
        };
        Batch* ring = newBatch(ringCol, GL_POINTS);
        midpointCircle(ring, (int)cx, (int)cy, radius);
    }

    // -----------------------------------------------------------------------
    //  Light rays — 8 radial lines emanating from sun centre
    //  More visible and warm near the horizon (sunrise/sunset effect)
    // -----------------------------------------------------------------------
    // Light rays — more at sunrise/sunset, subtle at noon
    float rayAlpha = (1.0f - heightFactor) * 0.28f + 0.03f;
    int   rayCount = 12;
    float rayLen   = SUN_R * (4.0f + (1.0f - heightFactor) * 5.5f);

    for (int i = 0; i < rayCount; ++i) {
        float angle = 2.0f * PI * i / rayCount;
        // Alternate long and short rays for natural starburst look
        float thisLen = (i % 2 == 0) ? rayLen : rayLen * 0.65f;
        float ex = cx + thisLen * std::cos(angle);
        float ey = cy + thisLen * std::sin(angle);
        float thisAlpha = (i % 2 == 0) ? rayAlpha : rayAlpha * 0.6f;

        glm::vec4 rayCol = {
            1.0f,
            0.80f + heightFactor * 0.15f,
            0.22f + heightFactor * 0.38f,
            thisAlpha
        };
        Batch* ray = newBatch(rayCol, GL_POINTS);
        ddaLine(ray, (int)cx, (int)cy, (int)ex, (int)ey);
    }

    // -----------------------------------------------------------------------
    //  Solid sun disc — warm yellow, more orange near horizon
    // -----------------------------------------------------------------------
    glm::vec4 discCol = {
        1.0f,
        0.82f + heightFactor * 0.12f,
        0.08f + heightFactor * 0.30f,
        1.0f
    };
    Batch* disc = newBatch(discCol, GL_TRIANGLE_FAN);
    filledCircle(disc, cx, cy, SUN_R);

    // -----------------------------------------------------------------------
    //  Bright core highlight
    // -----------------------------------------------------------------------
    Batch* core = newBatch({ 1.0f, 1.0f, 0.88f, 0.70f }, GL_TRIANGLE_FAN);
    filledCircle(core, cx + 6.0f, cy + 6.0f, SUN_R * 0.28f);
}

// ---------------------------------------------------------------------------
//  Accessors
// ---------------------------------------------------------------------------
glm::vec2 getSunPos() { return { g_sunX, g_sunY }; }
float     getSunT()   { return g_sunT; }
void      resetSun()  { g_sunT = 0.0f; }