#include "application.hpp"
#include "inputmanager.hpp"

#include "soup_mesh.hpp"
#include "half_edge_mesh.hpp"
#include "regular_mesh.hpp"
#include "glutil.hpp"

#include "GLFW/glfw3.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <iostream>
#include <string>

int main(int argc, char** argv)
{
	using namespace std::string_literals;
	if(argc < 1)
	{
		std::cerr << "Missing parameters. Expected path to model file.\n";
		return -1;
	}
	if(argc == 1 || argv[1] == "-h"s)
	{
		std::cout << "Usage:\n" << argv[0] << " <path> : Loads and displays model at path.\n"
			<< argv[0] << " -h : Shows this message.\n";
		return 0;
	}
	using namespace cg;

	auto app = Application{"Assignment 1", 640, 480};
	auto input = InputManager{};
	app.set_input(&input);
	auto window = app.get_window();

	// Load mesh
	auto mesh = SoupMesh{argv[1]};
	// Convert to half edge mesh
	auto hemesh = HalfEdgeMesh{mesh};
	// Convert back to renderable triangle soup
	mesh = hemesh.toSoupMesh();
	auto indices = mesh.calculate_indices();

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
	auto program = glCreateProgram();

	auto vs_path = std::string{"shaders/vertex_shader.glsl"};
	auto vs = glCreateShader(GL_VERTEX_SHADER);
	glutil::load_compile_shader(vs, {vs_path});
	glAttachShader(program, vs);

	auto fs_path = std::string{"shaders/fragment_shader.glsl"};
	auto fs = glCreateShader(GL_FRAGMENT_SHADER);
	glutil::load_compile_shader(fs, {fs_path});
	glAttachShader(program, fs);

	glLinkProgram(program);
	glUseProgram(program);
	glDetachShader(program, vs);
	glDetachShader(program, fs);

	auto mvp_uniform = glGetUniformLocation(program, "mvp");
	auto mvp = glm::mat4{1.f};
	auto sensitivity = 1.f;


	glEnable(GL_DEPTH_TEST);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	while(!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// TODO:DO STUFF
		if(input.get_key(GLFW_KEY_ESCAPE))
			glfwSetWindowShouldClose(window, GLFW_TRUE);

		mvp = glm::rotate(glm::mat4{1.f}, std::sin(static_cast<float>(glfwGetTime()) * sensitivity) * 0.5f, glm::vec3{1.f, 0.f, 0.f});
		mvp = glm::rotate(mvp, static_cast<float>(glfwGetTime()) * sensitivity * 1.0f, glm::vec3{0.f, 1.f, 0.f});
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
