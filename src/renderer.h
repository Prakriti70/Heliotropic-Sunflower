#pragma once
// =============================================================================
//  renderer.h
//  Lightweight 2D batch renderer.
//
//  All scene modules push geometry into Batch objects using newBatch()/addPt().
//  At the end of every frame, flushBatches() uploads all batches to the GPU
//  in a single sweep, draws them, then clears the list for the next frame.
//
//  Coordinate system: pixel space
//      x : 0 (left)   →  SCR_W (right)
//      y : 0 (bottom) →  SCR_H (top)
//  The orthographic projection matrix converts this to OpenGL NDC internally.
// =============================================================================

// Prevent Windows system headers from loading their own outdated GL declarations
// which conflict with GLAD. These must appear before glad/glad.h.
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOGDI
#define NOGDI
#endif

#include <glad/glad.h>        
#include <glm/glm.hpp>
#include <vector>
#include <string>

// Screen dimensions — used for the projection matrix.
// the GLFW window size set in main.cpp.
static const int SCR_W = 900;
static const int SCR_H = 600;

// =============================================================================
//  Batch
//  A group of 2D vertices that share the same colour and primitive mode.
//  Modules fill batches by calling addPt(); the renderer uploads them to GPU.
// =============================================================================
struct Batch {
    std::vector<float> verts;  // flat (x, y) pairs in pixel space
    glm::vec4          color;  // RGBA in [0, 1]
    GLenum             mode;   // GL_POINTS | GL_LINES | GL_TRIANGLE_FAN | etc.
};

// =============================================================================
//  Renderer lifetime
// =============================================================================

// Call once after a valid OpenGL context is created.
// Compiles the shader program, creates the VAO/VBO, sets up the projection.
// vertPath / fragPath : paths to the GLSL shader files on disk.
void initRenderer(const std::string& vertPath, const std::string& fragPath);

// Call at the end of every frame.
// Uploads and draws all pending batches, then clears the batch list.
void flushBatches();

// =============================================================================
//  Batch factory — used by every scene module
// =============================================================================

// Open a new batch with the given colour and OpenGL primitive mode.
// Returns a raw pointer valid until the next flushBatches() call.
Batch* newBatch(glm::vec4 color, GLenum mode);

// Append one 2D point (pixel space) to a batch.
void addPt(Batch* b, float x, float y);