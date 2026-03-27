#pragma once
// =============================================================================
//  scene.h
//  Top-level scene coordinator.
//
//  scene.cpp owns the simulation state (paused, speed) and ties together all
//  the individual modules (sky, clouds, hills, ground, sun, sunflower).
//
//  Keyboard controls (handled here):
//    SPACE       — pause / resume animation
//    +  / =      — speed up the sun
//    -           — slow down the sun
//    R           — reset sun to sunrise
//    ESC         — close the window
// =============================================================================

#include <GLFW/glfw3.h>

// Initialise all scene modules. Call once after OpenGL context is ready.
void initScene();

// Update simulation state for one frame.
// dt : seconds elapsed since last frame
void updateScene(float dt);

// Draw the complete scene for one frame (all modules, in correct order).
void drawScene();

// GLFW key callback — forward this from main.cpp's glfwSetKeyCallback.
void sceneKeyCallback(GLFWwindow* window, int key, int scancode,
                      int action, int mods);