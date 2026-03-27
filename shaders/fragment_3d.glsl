#version 330 core
// =============================================================================
//  fragment_3d.glsl  — Phong illumination model (Ch.6 of syllabus)
//
//  Implements:
//    - Ambient  reflection  : ka * Ia
//    - Diffuse  reflection  : kd * Id * max(dot(N,L), 0)
//    - Specular reflection  : ks * Is * pow(max(dot(R,V), 0), shininess)
//  where L = direction to light (sun), N = surface normal, V = view direction,
//        R = reflect(-L, N)
// =============================================================================

in vec3 vWorldPos;
in vec3 vNormal;
in vec4 vColor;

uniform vec3 uLightPos;    // sun position in world space
uniform vec3 uViewPos;     // camera position in world space
uniform vec3 uLightColor;  // sun light color (warm yellow)
uniform float uAmbient;    // ambient strength
uniform float uShininess;  // specular shininess

out vec4 FragColor;

void main() {
    vec3 N = normalize(vNormal);
    vec3 L = normalize(uLightPos - vWorldPos);
    vec3 V = normalize(uViewPos  - vWorldPos);
    vec3 R = reflect(-L, N);

    // Ambient
    vec3 ambient = uAmbient * uLightColor * vColor.rgb;

    // Diffuse
    float diff    = max(dot(N, L), 0.0);
    vec3  diffuse = diff * uLightColor * vColor.rgb;

    // Specular
    float spec     = pow(max(dot(R, V), 0.0), uShininess);
    vec3  specular = spec * uLightColor * vec3(1.0, 0.95, 0.7) * 0.35;

    vec3 result = ambient + diffuse + specular;
    FragColor   = vec4(result, vColor.a);
}
