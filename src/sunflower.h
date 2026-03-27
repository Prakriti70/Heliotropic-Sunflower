#pragma once
// =============================================================================
//  sunflower.h  — 3D heliotropic sunflower
// =============================================================================
#include <glm/glm.hpp>

void initSunflower();
void updateSunflower(glm::vec2 sunPos, float dt);

// Call BEFORE flushBatches() — queues the 2D shadow batch
void drawSunflower(float sunT);

// Call AFTER flushBatches() — draws the 3D mesh with Phong shading
void drawSunflower3D(float sunT);

glm::vec2 getStemTop();