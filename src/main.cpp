// =============================================================================
//  main.cpp
//  Entry point for the Heliotropic Sunflower simulation.
//
//  Responsibilities:
//    1. Initialise GLFW and create an OpenGL 3.3 core-profile window
//    2. Load GLAD (OpenGL function pointers)
//    3. Initialise the renderer (shader + VAO/VBO + projection)
//    4. Initialise the scene (all modules)
//    5. Run the render loop (update → draw → swap → poll)
//    6. Clean up on exit
//
//  Project: Heliotropic Sunflower — A 2D/3D Computer Graphics Simulation
//  Authors: Prakriti Poudel (081BCT055), Ritika Kc (081BCT067)
//  Campus : Tribhuvan University, IOE Pulchowk Campus
// =============================================================================

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "renderer.h" // initRenderer, flushBatches
#include "scene.h"    // initScene, updateScene, drawScene, sceneKeyCallback

// ---------------------------------------------------------------------------
//  Window configuration  (must match SCR_W / SCR_H in renderer.h)
// ---------------------------------------------------------------------------
static const int WIN_W = 900;
static const int WIN_H = 600;
static const char *TITLE = "Heliotropic Sunflower";

// Paths to the GLSL shader files (relative to the working directory,
// which CMake sets to the build folder — one level below FlowerBloom/).
static const char *VERT_PATH = "../shaders/vertex.glsl";//controls position and sends color to fragment shader
static const char *FRAG_PATH = "../shaders/fragment.glsl";//controls color

// ---------------------------------------------------------------------------
//  GLFW framebuffer resize callback
// ---------------------------------------------------------------------------
static void framebufferSizeCallback(GLFWwindow * /*window*/,//callback function to adjust the viewport
                                    int width, int height)
{
    glViewport(0, 0, width, height);
}

// ---------------------------------------------------------------------------
//  main
// ---------------------------------------------------------------------------
int main()
{
    // -----------------------------------------------------------------------
    //  1. Initialise GLFW
    // -----------------------------------------------------------------------
    if (!glfwInit())
    {
        std::cerr << "[GLFW] Initialisation failed.\n";
        return -1;
    }

    // Request OpenGL 3.3 core profile (required for GLAD / VAO)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // fixed-size window

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // -----------------------------------------------------------------------
    //  2. Create window
    // -----------------------------------------------------------------------
    GLFWwindow *window = glfwCreateWindow(WIN_W, WIN_H, TITLE, nullptr, nullptr);//creates the window
    if (!window)
    {
        std::cerr << "[GLFW] Window creation failed.\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);//Makes OpenGL work on this window
    glfwSwapInterval(1); // Enables smooth animation (vsync)

    // Register callbacks so that the window can respond to resizing and keyboard input
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetKeyCallback(window, sceneKeyCallback);

    // -----------------------------------------------------------------------
    //  3. Load OpenGL function pointers via GLAD
    // -----------------------------------------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "[GLAD] Failed to load OpenGL functions.\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    int fbW, fbH;
    glfwGetFramebufferSize(window, &fbW, &fbH);
    glViewport(0, 0, fbW, fbH);//Sets drawing area equal to window size

    std::cout << "OpenGL " << glGetString(GL_VERSION) << "\n";

    // -----------------------------------------------------------------------
    //  4. Initialise renderer and scene
    // -----------------------------------------------------------------------
    initRenderer(VERT_PATH, FRAG_PATH);
    initScene();

    // -----------------------------------------------------------------------
    //  5. Render loop
    // -----------------------------------------------------------------------
    float lastTime = (float)glfwGetTime();

    while (!glfwWindowShouldClose(window))
    {
        // --- Compute delta time -------------------------------------------
        float now = (float)glfwGetTime();
        float dt = now - lastTime;
        lastTime = now;

        // Clamp dt to avoid huge jumps after a pause or window drag
        if (dt > 0.1f)
            dt = 0.1f;

        // --- Update ----------------------------------------------------------
        updateScene(dt);

        // --- Draw ------------------------------------------------------------
        // Clear to a neutral dark colour (sky will cover it immediately)
        glClearColor(0.05f, 0.05f, 0.08f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        drawScene();    // builds all geometry into batches
        flushBatches(); // uploads batches to GPU and draws them

        // --- Swap and poll ---------------------------------------------------
        glfwSwapBuffers(window);//Shows the rendered image on screen
        glfwPollEvents();//to handle user input and window interactions
    }

    // -----------------------------------------------------------------------
    //  6. Clean up
    // -----------------------------------------------------------------------
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}