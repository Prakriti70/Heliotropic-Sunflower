#include "hills.h"
// =============================================================================
//  hills.cpp
//  Three wide, smooth hills drawn with quadratic Bézier curves.
//  The sun rises from behind the rightmost hill (sunrise effect).
//
//  Syllabus coverage:
//    - Chapter 4 : Parametric Bézier curves for hill silhouettes
//    - Chapter 3 : 2D window-to-viewport coordinate mapping
//    - Chapter 6 : Colour interpolation simulating ambient light change
// =============================================================================

#include "renderer.h"
#include "algorithms.h"
#include <glm/glm.hpp>
#include <cmath>
#include <vector>

static const float PI = 3.14159265358979f;

// ---------------------------------------------------------------------------
//  Quadratic Bézier evaluation
//  P(t) = (1-t)^2 * P0 + 2(1-t)t * P1 + t^2 * P2
// ---------------------------------------------------------------------------
static glm::vec2 bezier2(glm::vec2 p0, glm::vec2 p1, glm::vec2 p2, float t)
{
    float u = 1.0f - t;
    return u * u * p0 + 2.0f * u * t * p1 + t * t * p2;
}

// ---------------------------------------------------------------------------
//  drawHill
//  Fills the area under a quadratic Bézier arc down to y=0 (screen bottom).
//  Uses GL_TRIANGLE_FAN from a low anchor point — efficient fill.
// ---------------------------------------------------------------------------
static void drawHill(float p0x, float p0y, // left base
                     float p1x, float p1y, // control point (peak area)
                     float p2x, float p2y, // right base
                     glm::vec4 color,
                     glm::vec4 ridgeColor,
                     int steps = 80)
{
    // Use TWO Bézier segments meeting at the peak for a slightly pointed top:
    // Segment A: left base → peak (with control point left of peak)
    // Segment B: peak → right base (with control point right of peak)
    // The peak point = P1 (control point) which makes the top sharper
    glm::vec2 P0 = {p0x, p0y};
    glm::vec2 Pk = {p1x, p1y}; // actual peak point
    glm::vec2 P2 = {p2x, p2y};

    // Control points pull the curve toward the peak sharply
    float sharpness = 0.82f;                    // 1.0=very pointed, 0.5=round
    glm::vec2 CA = glm::mix(P0, Pk, sharpness); // left control
    glm::vec2 CB = glm::mix(P2, Pk, sharpness); // right control

    // --- Filled body -------------------------------------------------------
    Batch *fill = newBatch(color, GL_TRIANGLE_FAN);
    addPt(fill, p0x, 0.0f);

    // Left half: P0 → Pk using CA
    for (int i = 0; i <= steps / 2; ++i)
    {
        float t = (float)i / (float)(steps / 2);
        glm::vec2 pt = bezier2(P0, CA, Pk, t);
        addPt(fill, pt.x, pt.y);
    }
    // Right half: Pk → P2 using CB
    for (int i = 1; i <= steps / 2; ++i)
    {
        float t = (float)i / (float)(steps / 2);
        glm::vec2 pt = bezier2(Pk, CB, P2, t);
        addPt(fill, pt.x, pt.y);
    }
    addPt(fill, p2x, 0.0f);

    // --- Ridge outline -----------------------------------------------------
    Batch *ridge = newBatch(ridgeColor, GL_POINTS);
    for (int i = 0; i <= steps; ++i)
    {
        float t = (float)i / (float)steps;
        glm::vec2 pt;
        if (t <= 0.5f)
            pt = bezier2(P0, CA, Pk, t * 2.0f);
        else
            pt = bezier2(Pk, CB, P2, (t - 0.5f) * 2.0f);
        addPt(ridge, pt.x, pt.y);
    }
}

// ---------------------------------------------------------------------------
//  drawHills  –  public entry point
// ---------------------------------------------------------------------------
void drawHills(float sunT)
{
    // Golden-hour factor: 1 at sunrise/sunset, 0 at noon
    float gold = std::abs(sunT - 0.5f) * 2.0f;

    // -----------------------------------------------------------------------
    //  Layer 1 — Far hill (leftmost, smallest, palest — atmospheric depth)
    //  A single wide dome peaking left-of-centre
    // -----------------------------------------------------------------------
    {
        glm::vec4 noon = {0.09f, 0.24f, 0.06f, 1.0f};    // dark green
        glm::vec4 sunrise = {0.14f, 0.18f, 0.04f, 1.0f}; // dark olive
        glm::vec4 col = glm::mix(noon, sunrise, gold * 0.6f);
        glm::vec4 ridge = col * glm::vec4(0.75f, 0.75f, 0.75f, 1.0f);
        ridge.a = 0.45f;

        // Horizon sits at y=210; hill peak at y=260
        drawHill(-20.0f, 210.0f, // left base
                 280.0f, 340.0f, // raised peak — more height
                 640.0f, 210.0f, // right base
                 col, ridge);
    }

    // -----------------------------------------------------------------------
    //  Layer 2 — Mid hill (centre, medium height)
    // -----------------------------------------------------------------------
    {
        glm::vec4 noon = {0.12f, 0.32f, 0.08f, 1.0f};    // mid darkness
        glm::vec4 sunrise = {0.20f, 0.26f, 0.06f, 1.0f}; // mid olive
        glm::vec4 col = glm::mix(noon, sunrise, gold * 0.55f);
        glm::vec4 ridge = col * glm::vec4(0.72f, 0.72f, 0.72f, 1.0f);
        ridge.a = 0.50f;

        drawHill(-60.0f, 200.0f,
                 150.0f, 345.0f,
                 500.0f, 200.0f,
                 col, ridge);
    }

    // -----------------------------------------------------------------------
    //  Layer 3 — Right hill (tallest, sun rises from behind it)
    //  Peak is toward the right so the sun can crest it at sunrise
    // -----------------------------------------------------------------------
    {
        glm::vec4 noon = {0.15f, 0.38f, 0.11f, 1.0f };    // brightest — nearest
        glm::vec4 sunrise = {0.30f, 0.38f, 0.10f, 1.0f}; // warm bright olive
        glm::vec4 col = glm::mix(noon, sunrise, gold * 0.6f);
        glm::vec4 ridge = col * glm::vec4(0.70f, 0.70f, 0.70f, 1.0f);
        ridge.a = 0.55f;

        // Wide hill, peak at x≈700 (right side), sun crests here at sunrise
        drawHill(320.0f, 200.0f,
                 680.0f, 370.0f,
                 980.0f, 200.0f,
                 col, ridge);
    }
}