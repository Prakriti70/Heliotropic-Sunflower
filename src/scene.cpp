#define WIN32_LEAN_AND_MEAN
#define NOGDI
#include <glad/glad.h>
#include "scene.h"
// =============================================================================
//  scene.cpp
// =============================================================================
#include "sky.h"
#include "clouds.h"
#include "hills.h"
#include "ground.h"
#include "sunflower.h"
#include "sun.h"
#include "renderer.h"
#include <GLFW/glfw3.h>
#include <algorithm>
#include <iostream>

static bool  g_paused = false; 
static float g_speed  = 0.12f;
static const float SPEED_MIN  = 0.005f;
static const float SPEED_MAX  = 0.30f;
static const float SPEED_STEP = 0.005f;

void initScene() {
    initClouds();
    initSunflower();
    resetSun();
    std::cout << "=== Heliotropic Sunflower ===\n"
              << "  SPACE   : pause / resume\n"
              << "  + / =   : speed up\n"
              << "  -       : slow down\n"
              << "  R       : reset to sunrise\n"
              << "  ESC     : quit\n";
}

void updateScene(float dt) {
    if (g_paused) return;
    updateSun(dt, g_speed);
    updateClouds(dt);
    updateSunflower(getSunPos(), dt);
}

void drawScene() {
    float sunT = getSunT();

    // 2D batch layer
    drawSky(sunT);
    drawClouds(sunT);
    drawHills(sunT);
    drawGround(sunT);
    drawSunflower(sunT);   // queues 2D shadow only
    drawSun();
    flushBatches();        // flush all 2D to GPU

    // 3D layer — sunflower mesh with Phong shading
    glEnable(GL_DEPTH_TEST);
    glClear(GL_DEPTH_BUFFER_BIT);
    drawSunflower3D(sunT);
    glDisable(GL_DEPTH_TEST);
}

void sceneKeyCallback(GLFWwindow* window, int key, int /*sc*/,
                      int action, int /*mod*/) {
    if (action != GLFW_PRESS) return;
    switch (key) {
    case GLFW_KEY_SPACE:
        g_paused = !g_paused;
        std::cout << (g_paused ? "[Paused]\n" : "[Resumed]\n");
        break;
    case GLFW_KEY_EQUAL: case GLFW_KEY_KP_ADD:
        g_speed = std::min(g_speed + SPEED_STEP, SPEED_MAX);
        std::cout << "Speed: " << g_speed << "\n";
        break;
    case GLFW_KEY_MINUS: case GLFW_KEY_KP_SUBTRACT:
        g_speed = std::max(g_speed - SPEED_STEP, SPEED_MIN);
        std::cout << "Speed: " << g_speed << "\n";
        break;
    case GLFW_KEY_R:
        resetSun();
        initSunflower();
        std::cout << "[Reset to sunrise]\n";
        break;
    case GLFW_KEY_ESCAPE:
        glfwSetWindowShouldClose(window, GLFW_TRUE);
        break;
    default: break;
    }
}