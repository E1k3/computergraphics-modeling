cmake_minimum_required(VERSION 3.0)
project(assignments)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(assimp REQUIRED)
find_package(GLEW REQUIRED)
# find_package(glm REQUIRED)

# include_directories(${OPENGL_INCLUDE_DIR})

add_executable(assignment1 main.cpp application.cpp inputmanager.cpp)
target_link_libraries(assignment1 OpenGL::GL GLEW::GLEW glfw assimp)
target_compile_features(assignment1 PUBLIC cxx_std_14)
