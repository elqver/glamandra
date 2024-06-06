cmake_minimum_required(VERSION 3.10)
project(rb)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Find and add GLFW
find_package(glfw3 REQUIRED)
include_directories(${GLFW_INCLUDE_DIRS})

# Add GLAD
add_library(GLAD STATIC glad/src/glad.c)
include_directories(glad/include)

# Find and add OpenGL
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS})

# Add executable and link GLFW, OpenGL, and GLAD
add_executable(my_executable main.c)
target_link_libraries(my_executable glfw OpenGL::GL GLAD)
