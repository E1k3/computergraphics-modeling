cmake_minimum_required(VERSION 3.11)
project(assignments)


find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(assimp REQUIRED)
find_package(GLEW REQUIRED)
# glm config module is broken on archlinux atm
find_path(GLM_INCLUDE_DIR glm)
find_path(GSL_INCLUDE_DIR gsl)

if(GLM_INCLUDE_DIR STREQUAL "GLM_INCLUDE_DIR-NOTFOUND")
	message(FATAL_ERROR "Could not find OpenGL Mathematics library.")
endif()

if(GSL_INCLUDE_DIR STREQUAL "GSL_INCLUDE_DIR-NOTFOUND")
  message(FATAL_ERROR "Could not find Guidelines Support Library.")
endif()

# Assignment 1
add_executable(assignment1
	assignment1.cpp
	application.cpp
	inputmanager.cpp
	soup_mesh.cpp
	half_edge_mesh.cpp
	regular_mesh.cpp
	glutil.cpp)

target_include_directories(assignment1
	PUBLIC ${OPENGL_INCLUDE_DIR}
	PUBLIC ${GLEW_INCLUDE_DIRS}
	PUBLIC ${ASSIMP_INCLUDE_DIRS}
	PUBLIC ${GLM_INCLUDE_DIR}
	PUBLIC ${GSL_INCLUDE_DIR})

target_link_libraries(assignment1
	OpenGL::OpenGL
	GLEW::GLEW
	glfw
	assimp)

target_compile_features(assignment1 PUBLIC cxx_std_17)

target_compile_options(assignment1 PRIVATE -Wall -Wextra)

# Assignment 2
add_executable(assignment2
	assignment2.cpp
	application.cpp
	inputmanager.cpp
	soup_mesh.cpp
	half_edge_mesh.cpp
	regular_mesh.cpp
	glutil.cpp)

target_include_directories(assignment2
	PUBLIC ${OPENGL_INCLUDE_DIR}
	PUBLIC ${GLEW_INCLUDE_DIRS}
	PUBLIC ${ASSIMP_INCLUDE_DIRS}
	PUBLIC ${GLM_INCLUDE_DIR}
	PUBLIC ${GSL_INCLUDE_DIR})

target_link_libraries(assignment2
	OpenGL::OpenGL
	GLEW::GLEW
	glfw
	assimp)

target_compile_features(assignment2 PUBLIC cxx_std_17)

target_compile_options(assignment2 PRIVATE -Wall -Wextra)

# Assignment 3
add_executable(assignment3
	assignment3.cpp
	application.cpp
	inputmanager.cpp
	soup_mesh.cpp
	half_edge_mesh.cpp
	regular_mesh.cpp
	glutil.cpp)

target_include_directories(assignment3
	PUBLIC ${OPENGL_INCLUDE_DIR}
	PUBLIC ${GLEW_INCLUDE_DIRS}
	PUBLIC ${ASSIMP_INCLUDE_DIRS}
	PUBLIC ${GLM_INCLUDE_DIR}
	PUBLIC ${GSL_INCLUDE_DIR})

target_link_libraries(assignment3
	OpenGL::OpenGL
	GLEW::GLEW
	glfw
	assimp)

target_compile_features(assignment3 PUBLIC cxx_std_17)

target_compile_options(assignment3 PRIVATE -Wall -Wextra)

# Assets and shaders
add_custom_target(assignments)
add_dependencies(assignment1 assignments)
add_dependencies(assignment2 assignments)
add_dependencies(assignment3 assignments)

add_custom_command(TARGET assignments POST_BUILD 
	COMMAND ${CMAKE_COMMAND} -E create_symlink
	${CMAKE_SOURCE_DIR}/shaders
	${CMAKE_CURRENT_BINARY_DIR}/shaders)

add_custom_command(TARGET assignments POST_BUILD 
	COMMAND ${CMAKE_COMMAND} -E create_symlink
	${CMAKE_SOURCE_DIR}/assets
	${CMAKE_CURRENT_BINARY_DIR}/assets)
