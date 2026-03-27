#include "clouds.h"
// =============================================================================
//  clouds.cpp
//  Each cloud is drawn as 4 overlapping filled ellipses (puffs).
//  Uses filledEllipse() from algorithms.h for each puff.
// =============================================================================

#include "renderer.h"
#include "algorithms.h"
#include <glm/glm.hpp>
#include <array>
#include <cmath>

static const int CLOUD_COUNT = 5;

// ---------------------------------------------------------------------------
//  Cloud descriptor
// ---------------------------------------------------------------------------
struct Cloud {
    float x;      // x of cloud centre (pixel space)
    float y;      // y of cloud centre (fixed altitude per cloud)
    float speed;  // drift speed in pixels/second
    float scale;  // size multiplier (1.0 = default)
};

static std::array<Cloud, CLOUD_COUNT> g_clouds;

// ---------------------------------------------------------------------------
//  initClouds  –  place clouds at evenly staggered positions
// ---------------------------------------------------------------------------
void initClouds() {
    g_clouds[0] = {  80.0f, 490.0f, 16.0f, 1.00f };
    g_clouds[1] = { 280.0f, 515.0f, 11.0f, 0.75f };
    g_clouds[2] = { 530.0f, 470.0f, 20.0f, 1.20f };
    g_clouds[3] = { 720.0f, 500.0f, 14.0f, 0.85f };
    g_clouds[4] = { 170.0f, 450.0f,  9.0f, 0.65f };
}

// ---------------------------------------------------------------------------
//  updateClouds  –  drift right, wrap when fully off screen
// ---------------------------------------------------------------------------
void updateClouds(float dt) {
    for (auto& c : g_clouds) {
        c.x += c.speed * dt;
        if (c.x > (float)SCR_W + 130.0f)
            c.x = -130.0f;
    }
}

// ---------------------------------------------------------------------------
//  drawOneCloud  –  4 ellipse puffs arranged into a cloud silhouette
// ---------------------------------------------------------------------------
static void drawOneCloud(float cx, float cy, float scale, glm::vec4 color) {
    struct Puff { float dx, dy, rx, ry; };
    static const Puff puffs[] = {
        {   0.0f,  0.0f, 46.0f, 26.0f },   // main centre puff
        { -40.0f, -6.0f, 33.0f, 22.0f },   // left puff
        {  40.0f, -6.0f, 36.0f, 22.0f },   // right puff
        {   0.0f, 18.0f, 26.0f, 17.0f },   // top cap puff
    };

    for (auto& p : puffs) {
        Batch* b = newBatch(color, GL_TRIANGLE_FAN);
        filledEllipse(b,
                      cx + p.dx * scale,
                      cy + p.dy * scale,
                      p.rx * scale,
                      p.ry * scale);
    }
}

// ---------------------------------------------------------------------------
//  drawClouds  –  colour adapts sunrise(warm) → noon(white) → sunset(pink)
// ---------------------------------------------------------------------------
void drawClouds(float sunT) {
    // Colour key-frames
    static const glm::vec4 C_NOON    = { 1.00f, 1.00f, 1.00f, 0.88f };
    static const glm::vec4 C_SUNRISE = { 1.00f, 0.80f, 0.58f, 0.85f };
    static const glm::vec4 C_SUNSET  = { 1.00f, 0.68f, 0.48f, 0.85f };

    glm::vec4 cloudColor;
    if (sunT <= 0.5f)
        cloudColor = glm::mix(C_SUNRISE, C_NOON, sunT * 2.0f);
    else
        cloudColor = glm::mix(C_NOON, C_SUNSET, (sunT - 0.5f) * 2.0f);

    for (auto& c : g_clouds)
        drawOneCloud(c.x, c.y, c.scale, cloudColor);
}