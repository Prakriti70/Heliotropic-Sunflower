#include "mesh3d.h"
// =============================================================================
//  mesh3d.cpp
//  Mesh normal computation, GPU upload, and draw call.
// =============================================================================

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <sstream>
#include <iostream>

// ---------------------------------------------------------------------------
//  Mesh3D::computeFlatNormals
// ---------------------------------------------------------------------------
void Mesh3D::computeFlatNormals() {
    // Reset all normals
    for (auto& v : verts) v.normal = glm::vec3(0.0f);

    for (size_t i = 0; i + 2 < indices.size(); i += 3) {
        auto& v0 = verts[indices[i]];
        auto& v1 = verts[indices[i+1]];
        auto& v2 = verts[indices[i+2]];

        glm::vec3 e1 = v1.pos - v0.pos;
        glm::vec3 e2 = v2.pos - v0.pos;
        glm::vec3 n  = glm::normalize(glm::cross(e1, e2));

        v0.normal = n; v1.normal = n; v2.normal = n;
    }
}

// ---------------------------------------------------------------------------
//  Mesh3D::computeSmoothNormals
// ---------------------------------------------------------------------------
void Mesh3D::computeSmoothNormals() {
    for (auto& v : verts) v.normal = glm::vec3(0.0f);

    for (size_t i = 0; i + 2 < indices.size(); i += 3) {
        auto& v0 = verts[indices[i]];
        auto& v1 = verts[indices[i+1]];
        auto& v2 = verts[indices[i+2]];

        glm::vec3 e1 = v1.pos - v0.pos;
        glm::vec3 e2 = v2.pos - v0.pos;
        glm::vec3 n  = glm::cross(e1, e2);   // not normalized — weight by area

        v0.normal += n; v1.normal += n; v2.normal += n;
    }

    for (auto& v : verts) {
        if (glm::length(v.normal) > 0.0001f)
            v.normal = glm::normalize(v.normal);
        else
            v.normal = glm::vec3(0.0f, 0.0f, 1.0f);
    }
}

// ---------------------------------------------------------------------------
//  Shader helper
// ---------------------------------------------------------------------------
static GLuint compileShader3D(GLenum type, const std::string& src) {
    GLuint s = glCreateShader(type);
    const char* c = src.c_str();
    glShaderSource(s, 1, &c, nullptr);
    glCompileShader(s);
    int ok; glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[512]; glGetShaderInfoLog(s, 512, nullptr, log);
        std::cerr << "[3D Shader] " << log << "\n";
    }
    return s;
}

static std::string readFile3D(const std::string& path) {
    std::ifstream f(path);
    if (!f.is_open()) { std::cerr << "[3D] Cannot open: " << path << "\n"; return ""; }
    std::stringstream ss; ss << f.rdbuf(); return ss.str();
}

// ---------------------------------------------------------------------------
//  Mesh3DRenderer::init
// ---------------------------------------------------------------------------
void Mesh3DRenderer::init(const std::string& vertPath,
                           const std::string& fragPath) {
    std::string vs = readFile3D(vertPath);
    std::string fs = readFile3D(fragPath);

    GLuint v = compileShader3D(GL_VERTEX_SHADER,   vs);
    GLuint f = compileShader3D(GL_FRAGMENT_SHADER, fs);

    m_program = glCreateProgram();
    glAttachShader(m_program, v);
    glAttachShader(m_program, f);
    glLinkProgram(m_program);
    glDeleteShader(v); glDeleteShader(f);

    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_ebo);

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);

    // layout(0) = pos(3), layout(1) = normal(3), layout(2) = color(4)
    size_t stride = sizeof(Vertex3D);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(Vertex3D,pos));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(Vertex3D,normal));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(Vertex3D,color));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

// ---------------------------------------------------------------------------
//  Mesh3DRenderer::upload
// ---------------------------------------------------------------------------
void Mesh3DRenderer::upload(const Mesh3D& mesh) {
    m_indexCount = (GLsizei)mesh.indices.size();

    glBindVertexArray(m_vao);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 mesh.verts.size() * sizeof(Vertex3D),
                 mesh.verts.data(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 mesh.indices.size() * sizeof(unsigned int),
                 mesh.indices.data(), GL_DYNAMIC_DRAW);

    glBindVertexArray(0);
}

// ---------------------------------------------------------------------------
//  Mesh3DRenderer::draw
// ---------------------------------------------------------------------------
void Mesh3DRenderer::draw(const glm::mat4& model,
                           const glm::mat4& view,
                           const glm::mat4& proj,
                           const glm::vec3& lightPos,
                           const glm::vec3& viewPos,
                           const glm::vec3& lightColor,
                           float ambient,
                           float shininess) {
    glUseProgram(m_program);

    glm::mat3 normalMat = glm::transpose(glm::inverse(glm::mat3(model)));

    auto ul = [&](const char* n) { return glGetUniformLocation(m_program, n); };

    glUniformMatrix4fv(ul("uModel"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(ul("uView"),  1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(ul("uProj"),  1, GL_FALSE, glm::value_ptr(proj));
    glUniformMatrix3fv(ul("uNormalMat"),1,GL_FALSE,glm::value_ptr(normalMat));
    glUniform3fv(ul("uLightPos"),  1, glm::value_ptr(lightPos));
    glUniform3fv(ul("uViewPos"),   1, glm::value_ptr(viewPos));
    glUniform3fv(ul("uLightColor"),1, glm::value_ptr(lightColor));
    glUniform1f(ul("uAmbient"),    ambient);
    glUniform1f(ul("uShininess"),  shininess);

    glBindVertexArray(m_vao);
    glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Mesh3DRenderer::cleanup() {
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(1, &m_vbo);
    glDeleteBuffers(1, &m_ebo);
    glDeleteProgram(m_program);
}
