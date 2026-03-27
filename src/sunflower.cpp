#include "sunflower.h"
#define WIN32_LEAN_AND_MEAN
#define NOGDI
#include <glad/glad.h>
#include "renderer.h"
#include "algorithms.h"
#include "transforms.h"
#include "mesh3d.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
#include <vector>

static const float PI = 3.14159265358979f;

static const float STEM_BASE_X = 450.0f;
static const float STEM_BASE_Y = 200.0f;
static const float STEM_H = 130.0f;
static const float PULVINUS_Y = STEM_BASE_Y + STEM_H * 0.82f;
static const float HEAD_Y = STEM_BASE_Y + STEM_H;

static const float DISC_R = 26.0f;
static const int PETAL_N = 18;
static const float PETAL_LEN = 44.0f;
static const float PETAL_WID = 8.5f;
static const int INNER_N = 18;
static const float INNER_LEN = 28.0f;
static const float INNER_WID = 5.5f;
static const int SEED_COUNT = 89;
static const float GOLDEN_ANGLE = 2.39996f;

static const glm::vec3 CAM_POS = {450.0f, 310.0f, 700.0f};
static const glm::vec3 CAM_TARGET = {450.0f, 310.0f, 0.0f};
static const glm::vec3 CAM_UP = {0.0f, 1.0f, 0.0f};

static float g_yaw = 0.0f;
static float g_target = 0.0f;
static const float TRACK_SPEED = 2.5f;

static Mesh3D g_stemMesh;
static Mesh3D g_headMesh;
static Mesh3DRenderer g_stemRenderer;
static Mesh3DRenderer g_headRenderer;
static bool g_ready = false;

static void triAdd(Mesh3D &m, Vertex3D a, Vertex3D b, Vertex3D c)
{
    unsigned int i = (unsigned int)m.verts.size();
    m.verts.push_back(a);
    m.verts.push_back(b);
    m.verts.push_back(c);
    m.indices.push_back(i);
    m.indices.push_back(i + 1);
    m.indices.push_back(i + 2);
}
static void quadAdd(Mesh3D &m, Vertex3D a, Vertex3D b, Vertex3D c, Vertex3D d)
{
    triAdd(m, a, b, c);
    triAdd(m, a, c, d);
}

static void buildStemMesh()
{
    g_stemMesh.verts.clear();
    g_stemMesh.indices.clear();

    glm::vec4 stemLight = {0.22f, 0.55f, 0.14f, 1.0f};
    glm::vec4 stemDark = {0.14f, 0.40f, 0.08f, 1.0f};
    int N = 10;
    float rBot = 3.8f, rTop = 2.6f;
    for (int i = 0; i < N; ++i)
    {
        float a0 = 2 * PI * i / N, a1 = 2 * PI * (i + 1) / N;
        glm::vec3 b0 = {STEM_BASE_X + rBot * cosf(a0), STEM_BASE_Y, rBot * sinf(a0)};
        glm::vec3 b1 = {STEM_BASE_X + rBot * cosf(a1), STEM_BASE_Y, rBot * sinf(a1)};
        glm::vec3 t0 = {STEM_BASE_X + rTop * cosf(a0), PULVINUS_Y, rTop * sinf(a0)};
        glm::vec3 t1 = {STEM_BASE_X + rTop * cosf(a1), PULVINUS_Y, rTop * sinf(a1)};
        glm::vec3 n0 = glm::normalize(glm::vec3(cosf(a0), 0, sinf(a0)));
        glm::vec3 n1 = glm::normalize(glm::vec3(cosf(a1), 0, sinf(a1)));
        glm::vec4 c = (i % 3 == 0) ? stemDark : stemLight;
        quadAdd(g_stemMesh, {b0, n0, c}, {b1, n1, c}, {t1, n1, c}, {t0, n0, c});
    }

    auto buildLeaf = [&](float attachY, float azimuth, float len, float wid, float droop)
    {
        glm::vec4 topC = {0.16f, 0.46f, 0.10f, 1.0f}, botC = {0.10f, 0.30f, 0.06f, 1.0f};
        int SEG = 18;
        float ca = cosf(azimuth), sa = sinf(azimuth);
        for (int i = 0; i < SEG; ++i)
        {
            float t0 = (float)i / SEG, t1 = (float)(i + 1) / SEG;
            float lx0 = t0 * len, lx1 = t1 * len;
            float hw0 = sinf(powf(t0, 0.55f) * PI) * wid * (1.0f + 0.10f * sinf(t0 * PI * 3.0f));
            float hw1 = sinf(powf(t1, 0.55f) * PI) * wid * (1.0f + 0.10f * sinf(t1 * PI * 3.0f));
            float dz0 = droop * t0 * t0, dz1 = droop * t1 * t1;
            glm::vec3 p00 = {STEM_BASE_X + lx0 * ca + hw0 * (-sa), attachY - dz0, lx0 * sa + hw0 * ca};
            glm::vec3 p01 = {STEM_BASE_X + lx0 * ca - hw0 * (-sa) * 0.35f, attachY - dz0, lx0 * sa - hw0 * ca * 0.35f};
            glm::vec3 p10 = {STEM_BASE_X + lx1 * ca + hw1 * (-sa), attachY - dz1, lx1 * sa + hw1 * ca};
            glm::vec3 p11 = {STEM_BASE_X + lx1 * ca - hw1 * (-sa) * 0.35f, attachY - dz1, lx1 * sa - hw1 * ca * 0.35f};
            glm::vec3 nrm = glm::normalize(glm::vec3(-sa * 0.2f, 1.0f, ca * 0.2f));
            quadAdd(g_stemMesh, {p00, nrm, topC}, {p10, nrm, topC}, {p11, nrm, botC}, {p01, nrm, botC});
        }
    };

    buildLeaf(STEM_BASE_Y + STEM_H * 0.28f, PI * 0.55f, 56.0f, 21.0f, 9.0f);
    buildLeaf(STEM_BASE_Y + STEM_H * 0.52f, -PI * 0.18f, 60.0f, 23.0f, 11.0f);
    buildLeaf(STEM_BASE_Y + STEM_H * 0.68f, PI * 0.66f, 36.0f, 14.0f, 5.0f);

    g_stemMesh.computeSmoothNormals();
}

static void buildHeadMesh()
{
    g_headMesh.verts.clear();
    g_headMesh.indices.clear();

    // Outer petals
    {
        glm::vec4 colA = {1.00f, 0.82f, 0.00f, 1.0f}; // subtle golden yellow
        glm::vec4 colB = {0.95f, 0.72f, 0.00f, 1.0f}; // deeper gold

        glm::vec4 back = {0.92f, 0.68f, 0.05f, 1.0f};
        int STEPS = 16;
        for (int pi = 0; pi < PETAL_N; ++pi)
        {
            float pa = 2 * PI * pi / PETAL_N;
            glm::vec4 col = (pi % 3 == 0) ? colB : colA;
            float discEdge = DISC_R;
            std::vector<glm::vec3> upper(STEPS + 1), lower(STEPS + 1), norms(STEPS + 1);
            for (int s = 0; s <= STEPS; ++s)
            {
                float t = (float)s / STEPS;
                float lx = t * PETAL_LEN, lw = PETAL_WID * sinf(t * PI), lz = 8.0f * t * t;
                float wx = (discEdge + lx) * cosf(pa) - lw * 0.5f * sinf(pa);
                float wy = (discEdge + lx) * sinf(pa) + lw * 0.5f * cosf(pa);
                float wx2 = (discEdge + lx) * cosf(pa) + lw * 0.5f * sinf(pa);
                float wy2 = (discEdge + lx) * sinf(pa) - lw * 0.5f * cosf(pa);
                upper[s] = {wx, wy, lz};
                lower[s] = {wx2, wy2, lz};
                norms[s] = glm::normalize(glm::vec3(0, 0, 1) + glm::vec3(cosf(pa) * 0.15f, sinf(pa) * 0.15f, 0));
            }
            for (int s = 0; s < STEPS; ++s)
            {
                quadAdd(g_headMesh, {upper[s], norms[s], col}, {upper[s + 1], norms[s + 1], col},
                        {lower[s + 1], norms[s + 1], col}, {lower[s], norms[s], col});
                quadAdd(g_headMesh, {lower[s], -norms[s], back}, {lower[s + 1], -norms[s + 1], back},
                        {upper[s + 1], -norms[s + 1], back}, {upper[s], -norms[s], back});
            }
        }
    }

    // Inner petals
    {
        glm::vec4 col={1.00f,0.85f,0.05f,1.0f};
        glm::vec4 back = {0.92f, 0.70f, 0.08f, 1.0f};
        int STEPS = 12;
        for (int pi = 0; pi < INNER_N; ++pi)
        {
            float pa = 2 * PI * pi / INNER_N + (PI / INNER_N);
            float discEdge = DISC_R;
            std::vector<glm::vec3> upper(STEPS + 1), lower(STEPS + 1), norms(STEPS + 1);
            for (int s = 0; s <= STEPS; ++s)
            {
                float t = (float)s / STEPS;
                float lx = t * INNER_LEN, lw = INNER_WID * sinf(t * PI), lz = -1.5f + 5.0f * t * t;
                float wx = (discEdge + lx) * cosf(pa) - lw * 0.5f * sinf(pa);
                float wy = (discEdge + lx) * sinf(pa) + lw * 0.5f * cosf(pa);
                float wx2 = (discEdge + lx) * cosf(pa) + lw * 0.5f * sinf(pa);
                float wy2 = (discEdge + lx) * sinf(pa) - lw * 0.5f * cosf(pa);
                upper[s] = {wx, wy, lz};
                lower[s] = {wx2, wy2, lz};
                norms[s] = glm::normalize(glm::vec3(0, 0, 1) + glm::vec3(cosf(pa) * 0.15f, sinf(pa) * 0.15f, 0));
            }
            for (int s = 0; s < STEPS; ++s)
            {
                quadAdd(g_headMesh, {upper[s], norms[s], col}, {upper[s + 1], norms[s + 1], col},
                        {lower[s + 1], norms[s + 1], col}, {lower[s], norms[s], col});
                quadAdd(g_headMesh, {lower[s], -norms[s], back}, {lower[s + 1], -norms[s + 1], back},
                        {upper[s + 1], -norms[s + 1], back}, {upper[s], -norms[s], back});
            }
        }
    }

    // Disc + green backing + seeds
    {
        glm::vec4 discOuter = {0.22f, 0.10f, 0.02f, 1.0f};
        glm::vec4 discInner = {0.40f, 0.20f, 0.04f, 1.0f};
        glm::vec4 seed1 = {0.44f, 0.22f, 0.05f, 1.0f};
        glm::vec4 seed2 = {0.28f, 0.13f, 0.02f, 1.0f};
        glm::vec4 bMid = {0.18f, 0.46f, 0.10f, 1.0f};
        int sects = 32;
        float convex = 4.0f;

        // Green backing disc
        for (int s = 0; s < sects; ++s)
        {
            float a0 = 2 * PI * s / sects, a1 = 2 * PI * (s + 1) / sects;
            float gR = DISC_R * 0.80f;
            glm::vec3 ctr = {0, 0, -1.5f};
            glm::vec3 v0 = {gR * cosf(a0), gR * sinf(a0), -1.5f};
            glm::vec3 v1 = {gR * cosf(a1), gR * sinf(a1), -1.5f};
            glm::vec3 nrm = {0, 0, 1};
            triAdd(g_headMesh, {ctr, nrm, bMid}, {v0, nrm, bMid}, {v1, nrm, bMid});
        }
        // Brown disc front
        for (int s = 0; s < sects; ++s)
        {
            float a0 = 2 * PI * s / sects, a1 = 2 * PI * (s + 1) / sects;
            glm::vec3 ctr = {0, 0, convex};
            glm::vec3 v0 = {DISC_R * cosf(a0), DISC_R * sinf(a0), 0};
            glm::vec3 v1 = {DISC_R * cosf(a1), DISC_R * sinf(a1), 0};
            glm::vec3 nrm = {0, 0, 1};
            triAdd(g_headMesh, {ctr, nrm, discInner}, {v0, nrm, discOuter}, {v1, nrm, discOuter});
        }
        // Seeds
        for (int i = 1; i < SEED_COUNT; ++i)
        {
            float r = (DISC_R - 3.0f) * sqrtf((float)i / SEED_COUNT) * 0.90f;
            float phi = i * GOLDEN_ANGLE;
            float sx = r * cosf(phi), sy = r * sinf(phi), sz = convex + 1.5f;
            float sr = std::max(2.0f - 0.8f * ((float)i / SEED_COUNT), 0.7f);
            glm::vec4 sc = (i % 2 == 0) ? seed1 : seed2;
            glm::vec3 nrm = {0, 0, 1};
            glm::vec3 ctr = {sx, sy, sz};
            for (int j = 0; j < 5; ++j)
            {
                float ha0 = 2 * PI * j / 5.0f, ha1 = 2 * PI * (j + 1) / 5.0f;
                triAdd(g_headMesh, {ctr, nrm, sc},
                       {{sx + sr * cosf(ha0), sy + sr * sinf(ha0), sz}, nrm, sc},
                       {{sx + sr * cosf(ha1), sy + sr * sinf(ha1), sz}, nrm, sc});
            }
        }
    }

    g_headMesh.computeSmoothNormals();
}

void initSunflower()
{
    float sunriseNorm = (50.0f - 450.0f) / 400.0f;
    g_yaw = sunriseNorm * (PI * 0.48f);
    g_target = g_yaw;

    if (!g_ready)
    {
        g_stemRenderer.init("../shaders/vertex_3d.glsl", "../shaders/fragment_3d.glsl");
        g_headRenderer.init("../shaders/vertex_3d.glsl", "../shaders/fragment_3d.glsl");
        g_ready = true;
    }

    buildStemMesh();
    buildHeadMesh();
    g_stemRenderer.upload(g_stemMesh);
    g_headRenderer.upload(g_headMesh);
}

void updateSunflower(glm::vec2 sunPos, float dt)
{
    float sunNorm = glm::clamp((sunPos.x - 450.0f) / 400.0f, -1.0f, 1.0f);
    g_target = sunNorm * (PI * 0.48f);
    float safeDt = std::min(dt, 0.05f);
    float alpha = 1.0f - expf(-TRACK_SPEED * safeDt);
    float diff = g_target - g_yaw;
    while (diff > PI)
        diff -= 2.0f * PI;
    while (diff < -PI)
        diff += 2.0f * PI;
    g_yaw += diff * alpha;
}

glm::vec2 getStemTop()
{
    return {STEM_BASE_X + sinf(g_yaw) * 20.0f, PULVINUS_Y};
}

// drawSunflower — no shadow here, shadow drawn in 3D pass
void drawSunflower(float /*sunT*/) {}

void drawSunflower3D(float sunT)
{
    if (!g_ready)
        return;

    glm::mat4 view = glm::lookAt(CAM_POS, CAM_TARGET, CAM_UP);
    float aspect = (float)SCR_W / (float)SCR_H;
    glm::mat4 proj = glm::perspective(glm::radians(30.0f), aspect, 1.0f, 2000.0f);

    float sunH = sinf(sunT * PI);
    float sunAngle = PI * sunT;
    glm::vec3 lightPos = {
        450.0f + 400.0f * cosf(PI - sunAngle),
        320.0f + 280.0f * sinf(PI - sunAngle),
        400.0f};
    float gold = fabsf(sunT - 0.5f) * 2.0f;
    glm::vec3 lightCol = glm::mix(glm::vec3(1.0f, 1.0f, 0.94f),
                                  glm::vec3(1.0f, 0.62f, 0.18f), gold * 0.55f);
    float ambient   = 0.42f + gold * 0.06f;
    float shininess = 20.0f;

    // -----------------------------------------------------------------------
    //  Shadow — flat ellipse mesh ON the ground plane in 3D space
    //  Placed at y=STEM_BASE_Y, stretches opposite sun direction
    //  Drawn first so stem/head draw on top of it
    // -----------------------------------------------------------------------
    {
        // Smooth continuous shadow direction — no hard flip
        // sunT=0(left)→shadowDir=+1(right), sunT=1(right)→shadowDir=-1(left)
        // Use cos(PI*sunT) for smooth -1..+1 transition through noon
        float shadowDir = cosf(PI * sunT); // +1 at sunrise, 0 at noon, -1 at sunset
        float sLen = 55.0f - sunH * 32.0f;
        float sWid = 9.0f - sunH * 4.0f;
        float alpha = 0.45f + sunH * 0.10f;
        // At noon shadowDir≈0 so shadow is nearly centered — correct
        float shadowCX = STEM_BASE_X + shadowDir * sLen * 0.5f;
        float groundY = STEM_BASE_Y; // exact ground level

        Mesh3D shadowMesh;
        shadowMesh.verts.clear();
        shadowMesh.indices.clear();

        glm::vec3 upNrm = {0.0f, 1.0f, 0.0f};
        int sects = 28;

        // Outer soft ring
        glm::vec3 ctr1 = {shadowCX, groundY, 0.0f};
        for (int s = 0; s < sects; ++s)
        {
            float a0 = 2 * PI * s / sects, a1 = 2 * PI * (s + 1) / sects;
            glm::vec3 v0 = {shadowCX + sLen * 1.4f * cosf(a0), groundY, sWid * 1.7f * sinf(a0)};
            glm::vec3 v1 = {shadowCX + sLen * 1.4f * cosf(a1), groundY, sWid * 1.7f * sinf(a1)};
            glm::vec4 c = {0.01f, 0.05f, 0.01f, alpha * 0.45f};
            triAdd(shadowMesh, {ctr1, upNrm, c}, {v0, upNrm, c}, {v1, upNrm, c});
        }
        // Inner dark ellipse
        glm::vec4 darkC = {0.01f, 0.04f, 0.01f, alpha};
        for (int s = 0; s < sects; ++s)
        {
            float a0 = 2 * PI * s / sects, a1 = 2 * PI * (s + 1) / sects;
            glm::vec3 v0 = {shadowCX + sLen * cosf(a0), groundY, sWid * sinf(a0)};
            glm::vec3 v1 = {shadowCX + sLen * cosf(a1), groundY, sWid * sinf(a1)};
            triAdd(shadowMesh, {ctr1, upNrm, darkC}, {v0, upNrm, darkC}, {v1, upNrm, darkC});
        }

        shadowMesh.computeFlatNormals();

        // Use stem renderer temporarily to draw shadow
        static Mesh3DRenderer shadowRenderer;
        static bool shadowReady = false;
        if (!shadowReady)
        {
            shadowRenderer.init("../shaders/vertex_3d.glsl",
                                "../shaders/fragment_3d.glsl");
            shadowReady = true;
        }
        shadowRenderer.upload(shadowMesh);
        shadowRenderer.draw(glm::mat4(1.0f), view, proj,
                            lightPos, CAM_POS, lightCol, 0.02f, 1.0f);
    }

    // --- Stem ---
    g_stemRenderer.draw(glm::mat4(1.0f), view, proj,
                        lightPos, CAM_POS, lightCol, ambient, shininess);

    // --- Head (Y-axis rotation) ---
    // Enable back-face culling to prevent petal z-fighting flicker
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    glm::mat4 headModel = glm::mat4(1.0f);
    headModel = glm::translate(headModel, glm::vec3(STEM_BASE_X, HEAD_Y, 0.0f));
    headModel = glm::rotate(headModel, g_yaw, glm::vec3(0.0f, 1.0f, 0.0f));
    g_headRenderer.draw(headModel, view, proj,
                        lightPos, CAM_POS, lightCol, ambient, shininess);

    glDisable(GL_CULL_FACE);
}