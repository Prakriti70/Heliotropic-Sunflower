#pragma once
// =============================================================================
//  mesh3d.h
//  Minimal 3D mesh renderer used exclusively for the sunflower.
//  Keeps the 2D batch renderer untouched — the 3D pipeline runs in parallel.
//
//  Vertex format:  position(3) + normal(3) + color(4) = 10 floats
//  Draw call:      indexed triangles (GL_TRIANGLES + EBO)
// =============================================================================

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>

// ---------------------------------------------------------------------------
//  Vertex3D
// ---------------------------------------------------------------------------
struct Vertex3D {
    glm::vec3 pos;
    glm::vec3 normal;
    glm::vec4 color;
};

// ---------------------------------------------------------------------------
//  Mesh3D  — uploadable triangle mesh
// ---------------------------------------------------------------------------
struct Mesh3D {
    std::vector<Vertex3D>    verts;
    std::vector<unsigned int> indices;

    // Compute flat normals from triangle indices (overwrites existing normals)
    void computeFlatNormals();

    // Compute smooth normals (average of adjacent triangle normals)
    void computeSmoothNormals();
};

// ---------------------------------------------------------------------------
//  Mesh3DRenderer  — owns the VAO/VBO/EBO and 3D shader program
// ---------------------------------------------------------------------------
class Mesh3DRenderer {
public:
    void init(const std::string& vertPath, const std::string& fragPath);

    // Upload mesh data to GPU (call whenever mesh changes)
    void upload(const Mesh3D& mesh);

    // Draw with given transforms and lighting
    void draw(const glm::mat4& model,
              const glm::mat4& view,
              const glm::mat4& proj,
              const glm::vec3& lightPos,
              const glm::vec3& viewPos,
              const glm::vec3& lightColor,
              float ambient,
              float shininess);

    void cleanup();

private:
    GLuint m_vao     = 0;
    GLuint m_vbo     = 0;
    GLuint m_ebo     = 0;
    GLuint m_program = 0;
    GLsizei m_indexCount = 0;
};
