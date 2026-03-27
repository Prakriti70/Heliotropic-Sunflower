#version 330 core
// =============================================================================
//  vertex_3d.glsl  — used ONLY for the 3D sunflower mesh
//  Transforms 3D vertices through model→view→projection pipeline.
//  Passes world-space position and normal to fragment shader for Phong lighting.
// =============================================================================

layout (location = 0) in vec3 aPos;      // vertex position (model space)
layout (location = 1) in vec3 aNormal;   // vertex normal   (model space)
layout (location = 2) in vec4 aColor;    // per-vertex base color

uniform mat4 uModel;      // model transform (rotation + translation)
uniform mat4 uView;       // view  transform (camera)
uniform mat4 uProj;       // projection (perspective)
uniform mat3 uNormalMat;  // transpose(inverse(model)) for correct normals

out vec3 vWorldPos;
out vec3 vNormal;
out vec4 vColor;

void main() {
    vec4 worldPos = uModel * vec4(aPos, 1.0);
    vWorldPos = worldPos.xyz;
    vNormal   = normalize(uNormalMat * aNormal);
    vColor    = aColor;
    gl_Position = uProj * uView * worldPos;
}
