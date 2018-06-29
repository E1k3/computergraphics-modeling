#include "application.hpp"
#include "inputmanager.hpp"

#include "soup_mesh.hpp"
#include "half_edge_mesh.hpp"
#include "glutil.hpp"

#include "GLFW/glfw3.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <iostream>

int main(int /*argc*/, char** /*argv*/)
{
	using namespace cg;

	Application app{"Assignment 4", 640, 480};
	InputManager input{};
	app.set_input(&input);
	GLFWwindow* window{app.get_window()};

	// Calculate positions
	std::vector<glm::vec3> positions{{0.f, 0.f, 0.f}, {1.f, 0.f, 0.f}, {0.f, 1.f, 0.f}, {1.f, 1.f, 0.f}, {1.f, 1.f, 1.f}};
	std::vector<std::vector<unsigned int>> faces{{0,2,1}, {0,1,4}, {0,4,2}, {1,2,3}, {1,3,4}, {2,4,3}};
	SoupMesh mesh{positions, {}, {}, faces};

	auto indices{mesh.calculate_indices()};

	GLuint vao;
	GLuint vbo[2];
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(2, vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * mesh.get_positions().size(), mesh.get_positions().data(), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), indices.data(), GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	// Load shader
	auto program{glCreateProgram()};

	std::string vs_path{"shaders/vertex_shader.glsl"};
	auto vs{glCreateShader(GL_VERTEX_SHADER)};
	glutil::load_compile_shader(vs, {vs_path});
	glAttachShader(program, vs);

	std::string fs_path{"shaders/fragment_shader.glsl"};
	auto fs{glCreateShader(GL_FRAGMENT_SHADER)};
	glutil::load_compile_shader(fs, {fs_path});
	glAttachShader(program, fs);

	glLinkProgram(program);
	glUseProgram(program);
	glDetachShader(program, vs);
	glDetachShader(program, fs);

	auto mvp_uniform{glGetUniformLocation(program, "mvp")};
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glm::mat4 model{glm::translate(glm::mat4{1.f}, glm::vec3{-0.5f, -0.5f, -0.5f})};
	glm::mat4 view{glm::scale(glm::mat4{1.f}, glm::vec3{.5f, .5f, .5f})};
	glm::mat4 project{glm::perspective(glm::radians(75.f), static_cast<float>(width)/height, .1f, 100.f)};
	glm::mat4 mvp{1.f};
	float sensitivity{.005f};


	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	while(!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// TODO:DO STUFF
		// Rotate mesh while holding MOUSE2
		if(input.get_key(GLFW_MOUSE_BUTTON_2))
		{
			view = glm::rotate(glm::mat4{1.f}, input.get_cursor_offset().x * sensitivity, glm::vec3{0.f, 1.f, 0.f}) * view;
			view = glm::rotate(glm::mat4{1.f}, input.get_cursor_offset().y * sensitivity, glm::vec3{1.f, 0.f, 0.f}) * view;
		}
		
		if(input.get_key(GLFW_MOUSE_BUTTON_1))
		{
			// TODO: 1. state: remove vertex closest to cursor, 2. state: move vertex closest to cursor
		}
		
		mvp = project * glm::translate(glm::mat4{1.f}, glm::vec3{0.f, 0.f, -1.f}) * view * model;
		glUniformMatrix4fv(mvp_uniform, 1, GL_FALSE, value_ptr(mvp));
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);
		
		glfwSwapBuffers(window);
		input.unstick();
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(2, vbo);
	glDeleteShader(vs);
	glDeleteShader(fs);
	glDeleteProgram(program);

	return 0;
}
