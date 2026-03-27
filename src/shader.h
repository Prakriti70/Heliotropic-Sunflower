#pragma once
// =============================================================================
//  shader.h
//  Loads GLSL source files from disk and compiles/links them into an OpenGL
//  shader program.
//
//  Usage:
//      GLuint prog = loadShaderProgram("shaders/vertex.glsl",
//                                      "shaders/fragment.glsl");
// =============================================================================

#include <glad/glad.h>
#include <string>

// Read a text file from disk and return its contents as a string.
// Returns empty string and prints an error if the file cannot be opened.
std::string readFile(const std::string& path);

// Compile a single shader stage from source text.
// type  : GL_VERTEX_SHADER or GL_FRAGMENT_SHADER
// src   : GLSL source as a null-terminated C string
// Returns the shader object ID, or 0 on failure.
GLuint compileShader(GLenum type, const char* src);

// Load, compile and link a complete shader program from two GLSL files.
// vertPath : path to the vertex shader file
// fragPath : path to the fragment shader file
// Returns the program ID, or 0 on failure.
GLuint loadShaderProgram(const std::string& vertPath,
                         const std::string& fragPath);