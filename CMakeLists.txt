cmake_minimum_required(VERSION 3.15)
project(FlowerBloom CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MINGW)
    set(GLFW_LIB_DIR "${CMAKE_SOURCE_DIR}/lib-mingw")
else()
    set(GLFW_LIB_DIR "${CMAKE_SOURCE_DIR}/lib-vc-2022")
endif()

set(SOURCES
    src/glad.c
    src/main.cpp
    src/shader.cpp
    src/renderer.cpp
    src/mesh3d.cpp
    src/algorithms.cpp
    src/transforms.cpp
    src/sky.cpp
    src/sun.cpp
    src/clouds.cpp
    src/hills.cpp
    src/ground.cpp
    src/sunflower.cpp
    src/scene.cpp
)

add_executable(FlowerBloom ${SOURCES})

set_source_files_properties(src/glad.c PROPERTIES
    COMPILE_DEFINITIONS "NOGDI;WIN32_LEAN_AND_MEAN"
)

target_compile_definitions(FlowerBloom PRIVATE
    WIN32_LEAN_AND_MEAN
    NOGDI
)

target_include_directories(FlowerBloom PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
)

target_link_directories(FlowerBloom PRIVATE
    ${GLFW_LIB_DIR}
)

if(WIN32)
    target_link_libraries(FlowerBloom PRIVATE
        glfw3 opengl32 gdi32 user32 shell32
    )
else()
    target_link_libraries(FlowerBloom PRIVATE
        glfw GL dl pthread
    )
endif()

# Copy shaders to build output
add_custom_command(TARGET FlowerBloom POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/shaders"
        "$<TARGET_FILE_DIR:FlowerBloom>/../shaders"
    COMMENT "Copying shaders"
)