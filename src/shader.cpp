#include <glad/glad.h>   // MUST be first
#include "shader.h"
// =============================================================================
//  shader.cpp
//  Reads GLSL files from disk and compiles/links them into an OpenGL program.
// =============================================================================

#include <iostream>
#include <fstream>
#include <sstream>

// -----------------------------------------------------------------------------
//  readFile
//  Opens a text file and returns the entire contents as a std::string.
// -----------------------------------------------------------------------------
std::string readFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "[Shader] Cannot open file: " << path << "\n";
        return "";
    }
    std::stringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

// -----------------------------------------------------------------------------
//  compileShader
//  Allocates a GL shader object, uploads source, compiles, and checks status.
// -----------------------------------------------------------------------------
GLuint compileShader(GLenum type, const char* src) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char log[512];
        glGetShaderInfoLog(shader, 512, nullptr, log);
        std::cerr << "[Shader Compile Error]\n" << log << "\n";
        return 0;
    }
    return shader;
}

// -----------------------------------------------------------------------------
//  loadShaderProgram
//  Full pipeline: read files → compile stages → link program → return ID.
// -----------------------------------------------------------------------------
GLuint loadShaderProgram(const std::string& vertPath,
                         const std::string& fragPath) {
    // Read source files
    std::string vertSrc = readFile(vertPath);
    std::string fragSrc = readFile(fragPath);

    if (vertSrc.empty() || fragSrc.empty()) {
        std::cerr << "[Shader] Failed to read shader source files.\n";
        return 0;
    }

    // Compile individual stages
    GLuint vs = compileShader(GL_VERTEX_SHADER,   vertSrc.c_str());
    GLuint fs = compileShader(GL_FRAGMENT_SHADER, fragSrc.c_str());

    // Link into program
    GLuint program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    int success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char log[512];
        glGetProgramInfoLog(program, 512, nullptr, log);
        std::cerr << "[Shader Link Error]\n" << log << "\n";
    }

    // Individual shader objects are no longer needed after linking
    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}