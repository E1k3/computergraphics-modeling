cmake_minimum_required(VERSION 3.0)
project(assignments)

# Make FindOpenGL use GLVND, if GLVND and LEGACY are available
if(POLICY CMP0072)
  cmake_policy(SET CMP0072 NEW)
endif()

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(assimp REQUIRED)
find_package(GLEW REQUIRED)
# glm config module is broken on archlinux atm
# find_package(glm REQUIRED)


add_executable(assignment1
	main.cpp
	application.cpp
	inputmanager.cpp
	soup_mesh.cpp
	half_edge_mesh.cpp
	glutil.cpp)

target_include_directories(assignment1
	PUBLIC ${OPENGL_INCLUDE_DIR}
	PUBLIC ${GLEW_INCLUDE_DIRS}
	PUBLIC ${ASSIMP_INCLUDE_DIRS})

target_link_libraries(assignment1
	OpenGL::GL
	GLEW::GLEW
	glfw
	assimp)

target_compile_features(assignment1 PUBLIC cxx_std_14)

target_compile_options(assignment1 PRIVATE -Wall)

add_custom_command(TARGET assignment1 POST_BUILD 
	COMMAND ${CMAKE_COMMAND} -E copy_directory
	${CMAKE_SOURCE_DIR}/shaders/
	${CMAKE_CURRENT_BINARY_DIR}/shaders/)

add_custom_command(TARGET assignment1 POST_BUILD 
	COMMAND ${CMAKE_COMMAND} -E copy_directory
	${CMAKE_SOURCE_DIR}/assets/
	${CMAKE_CURRENT_BINARY_DIR}/assets/)
