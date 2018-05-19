cmake_minimum_required(VERSION 3.11)
project(assignments)


find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(assimp REQUIRED)
find_package(GLEW REQUIRED)
# glm config module is broken on archlinux atm
# find_package(glm REQUIRED)


add_executable(assignment
	main.cpp
	application.cpp
	inputmanager.cpp
	soup_mesh.cpp
	half_edge_mesh.cpp
	regular_mesh.cpp
	glutil.cpp)

target_include_directories(assignment
	PUBLIC ${OPENGL_INCLUDE_DIR}
	PUBLIC ${GLEW_INCLUDE_DIRS}
	PUBLIC ${ASSIMP_INCLUDE_DIRS})

target_link_libraries(assignment
	OpenGL::OpenGL
	GLEW::GLEW
	glfw
	assimp)

target_compile_features(assignment PUBLIC cxx_std_17)

target_compile_options(assignment PRIVATE -Wall -Wextra)

add_custom_command(TARGET assignment POST_BUILD 
	COMMAND ${CMAKE_COMMAND} -E copy_directory
	${CMAKE_SOURCE_DIR}/shaders/
	${CMAKE_CURRENT_BINARY_DIR}/shaders/)

add_custom_command(TARGET assignment POST_BUILD 
	COMMAND ${CMAKE_COMMAND} -E copy_directory
	${CMAKE_SOURCE_DIR}/assets/
	${CMAKE_CURRENT_BINARY_DIR}/assets/)
