// Prevent Windows headers from pulling in outdated OpenGL declarations
#define WIN32_LEAN_AND_MEAN
#define NOGDI
#include <glad/glad.h>   // MUST be first — loads all OpenGL function pointers
#include "renderer.h"
// =============================================================================
//  renderer.cpp
//  One persistent VAO + VBO pair is reused for every draw call.
//  Data is streamed to the GPU with GL_DYNAMIC_DRAW each batch.
//  The orthographic projection converts pixel coords to NDC each frame.
// =============================================================================

#include "shader.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>

// ---------------------------------------------------------------------------
//  Module-private OpenGL state
// ---------------------------------------------------------------------------
static GLuint             g_program = 0;
static GLuint             g_vao     = 0;
static GLuint             g_vbo     = 0;
static glm::mat4          g_proj;
static std::vector<Batch> g_batches;

// Cached uniform locations (set once after program is built)
static GLint g_mvpLoc   = -1;
static GLint g_colorLoc = -1;

// ---------------------------------------------------------------------------
//  initRenderer
// ---------------------------------------------------------------------------
void initRenderer(const std::string& vertPath, const std::string& fragPath) {
    // Build the shader program from the .glsl files on disk
    g_program = loadShaderProgram(vertPath, fragPath);

    // Cache uniform locations
    g_mvpLoc   = glGetUniformLocation(g_program, "uMVP");
    g_colorLoc = glGetUniformLocation(g_program, "uColor");

    // Create one VAO + VBO; vertex data is re-uploaded every draw call
    glGenVertexArrays(1, &g_vao);
    glGenBuffers(1, &g_vbo);

    glBindVertexArray(g_vao);
    glBindBuffer(GL_ARRAY_BUFFER, g_vbo);

    // layout(location = 0): 2 floats per vertex, tightly packed
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE,
                          2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    // Orthographic projection: maps pixel space (0..SCR_W, 0..SCR_H) to NDC
    g_proj = glm::ortho(0.0f, (float)SCR_W,
                        0.0f, (float)SCR_H,
                        -1.0f, 1.0f);

    // Slightly larger points so rasterised pixels are clearly visible
    glPointSize(1.8f);

    // Enable alpha blending for glow effects and transparent shadows
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

// ---------------------------------------------------------------------------
//  newBatch  –  open a new batch and return a pointer to it
// ---------------------------------------------------------------------------
Batch* newBatch(glm::vec4 color, GLenum mode) {
    g_batches.push_back({});
    Batch* b = &g_batches.back();
    b->color = color;
    b->mode  = mode;
    return b;
}

// ---------------------------------------------------------------------------
//  addPt  –  append one pixel-space vertex to a batch
// ---------------------------------------------------------------------------
void addPt(Batch* b, float x, float y) {
    b->verts.push_back(x);
    b->verts.push_back(y);
}

// ---------------------------------------------------------------------------
//  flushBatches  –  draw everything then clear
// ---------------------------------------------------------------------------
void flushBatches() {
    if (g_batches.empty()) return;

    glUseProgram(g_program);

    // Upload the projection matrix once per frame (same for all batches)
    glUniformMatrix4fv(g_mvpLoc, 1, GL_FALSE, glm::value_ptr(g_proj));

    glBindVertexArray(g_vao);

    for (auto& b : g_batches) {
        if (b.verts.empty()) continue;

        // Set this batch's colour
        glUniform4fv(g_colorLoc, 1, glm::value_ptr(b.color));

        // Stream vertex data to GPU (dynamic since it changes every frame)
        glBindBuffer(GL_ARRAY_BUFFER, g_vbo);
        glBufferData(GL_ARRAY_BUFFER,
                     (GLsizeiptr)(b.verts.size() * sizeof(float)),
                     b.verts.data(),
                     GL_DYNAMIC_DRAW);

        glDrawArrays(b.mode, 0, (GLsizei)(b.verts.size() / 2));
    }

    glBindVertexArray(0);

    // Clear all batches — ready for the next frame
    g_batches.clear();
}