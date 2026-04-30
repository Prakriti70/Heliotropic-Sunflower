HELIOTROPIC SUNFLOWER SIMULATION
 
OVERVIEW

This project is a 3D simulation of a sunflower that follows the sun (heliotropism) using OpenGL and C++.
It demonstrates computer graphics concepts like transformations, interpolation, shading, and animation.

FEATURES
 Sun movement across the scene
 Sunflower tracks the sun (heliotropic behavior)
 Smooth color interpolation (day → sunset → night)
 Cloud animation
 Basic shading and lighting effects
 Use of mathematical concepts (vectors, matrices, interpolation)

TECHNOLOGIES USED:
C++
OpenGL
GLFW / GLAD
CMake
GLM (for math)

PROJECT STRUCTURE
FlowerBloom/
│── include/
│── src/
│── shaders/
│── assets/
│── CMakeLists.txt
│── .gitignore
│── README.md


HOW TO RUN?
-Install CMake
I-nstall a C++ compiler (MinGW / GCC / MSVC)
-Install OpenGL dependencies (GLFW, GLAD, etc.)
-Build Instructions:
      mkdir build
      cd build
      cmake ..
      cmake --build
-Run
  ./FlowerBloom.exe

CONCEPTS IMPLEMENTED:
-Transformation (translation, rotation, scaling)
-Interpolation (color & movement)
-Camera/View (lookAt)
-Rendering pipeline
-Shading techniques

 

This project helped me understand:

-Animation in graphics
-Real-time rendering
-Mathematical modeling of natural behavior






