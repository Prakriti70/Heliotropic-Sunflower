#include "ground.h"
#include "renderer.h"
#include "algorithms.h"
#include <glm/glm.hpp>
#include <cmath>

static const int GROUND_TOP = 200;
static const float PI = 3.14159265358979f;

void drawGround(float sunT) {
    float gold = std::abs(sunT - 0.5f) * 2.0f;
    float tintF = 1.0f - gold * 0.20f;

    // Uniform ground fill
    glm::vec4 groundNoon    = { 0.20f, 0.52f, 0.14f, 1.0f };
    glm::vec4 groundSunrise = { 0.25f, 0.32f, 0.07f, 1.0f };
    glm::vec4 groundColor   = glm::mix(groundNoon, groundSunrise, gold * 0.5f);

    Batch* fill = newBatch(groundColor, GL_TRIANGLE_FAN);
    addPt(fill, 0.0f,         0.0f);
    addPt(fill, (float)SCR_W, 0.0f);
    addPt(fill, (float)SCR_W, (float)GROUND_TOP);
    addPt(fill, 0.0f,         (float)GROUND_TOP);

    // Grass + flower colors
    glm::vec4 grassDark = { 0.22f*tintF, 0.52f*tintF, 0.13f*tintF, 1.0f };
    glm::vec4 grassTip  = { 0.38f*tintF, 0.72f*tintF, 0.20f*tintF, 1.0f };

    glm::vec4 petalWhite  = { 0.95f*tintF, 0.95f*tintF, 0.95f*tintF, 1.0f };
    glm::vec4 petalPink   = { 0.95f*tintF, 0.62f*tintF, 0.72f*tintF, 1.0f };
    glm::vec4 petalYellow = { 0.98f*tintF, 0.90f*tintF, 0.22f*tintF, 1.0f };
    glm::vec4 petalPurple = { 0.68f*tintF, 0.48f*tintF, 0.92f*tintF, 1.0f };
    glm::vec4 centerYellow= { 0.98f*tintF, 0.82f*tintF, 0.08f*tintF, 1.0f };
    glm::vec4 petalColors[] = { petalWhite, petalPink, petalYellow, petalPurple };

    // Sparse grass strands every 14px, some with flowers at the tip
    for (int x = 6; x < SCR_W; x += 14) {
        int h    = 10 + (x * 17 + 7) % 12;
        int lean = ((x * 13 + 5) % 9) - 4;
        int tipX = x + lean;
        int tipY = GROUND_TOP + h;

        Batch* stem = newBatch(grassDark, GL_POINTS);
        bresenhamLine(stem, x, GROUND_TOP, tipX, tipY);

        int midY = GROUND_TOP + h * 2 / 3;
        Batch* stemTop = newBatch(grassTip, GL_POINTS);
        bresenhamLine(stemTop, x + lean/2, midY, tipX, tipY);

        if ((x / 14) % 3 == 0) {
            int colorType = (x / 14) % 4;
            glm::vec4 pc  = petalColors[colorType];
            float petalR  = 2.5f;
            for (int p = 0; p < 5; ++p) {
                float angle = 2.0f * PI * p / 5.0f - PI * 0.5f;
                float px = tipX + petalR * 1.7f * cosf(angle);
                float py = tipY + petalR * 1.7f * sinf(angle);
                Batch* petal = newBatch(pc, GL_TRIANGLE_FAN);
                filledCircle(petal, px, py, petalR, 5);
            }
            Batch* center = newBatch(centerYellow, GL_TRIANGLE_FAN);
            filledCircle(center, (float)tipX, (float)tipY, petalR * 0.8f, 5);
        }
        else if ((x / 14) % 5 == 1) {
            int colorType = (x / 42) % 4;
            Batch* bud = newBatch(petalColors[colorType], GL_TRIANGLE_FAN);
            filledCircle(bud, (float)tipX, (float)tipY, 2.2f, 5);
        }
    }
}