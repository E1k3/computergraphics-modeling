#include "application.hpp"

#include "soup_mesh.hpp"
#include "half_edge_mesh.hpp"
#include "glutil.hpp"

#include "glm/glm.hpp"

#include <iostream>

namespace cg
{
	Application::Application(std::string title, int window_width, int window_height)
		: title{title},
		  width{window_width},
		  height{window_height}
	{
		// GLFW init
		glfwSetErrorCallback(error_callback);
		if(!glfwInit())
		{
			std::cerr << "GLFW failed to initialize\n";
			throw std::runtime_error{"GLFW failed to initialize."};
		}


		// Window settings
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);	// Set Opengl constext version 3.3
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);	// Set OpenGL core profile
		glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);	// Make window visible on creation
		glfwWindowHint(GLFW_SAMPLES, 16);	// Anti-aliasing


		// Window init
		constexpr auto window_deleter = [] (GLFWwindow* window) { glfwDestroyWindow(window); glfwTerminate(); };
		window = WindowPointer{glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr), window_deleter};
		
		if(!window)
		{
			std::cerr << "GLFW window creation failed\n";
			throw std::runtime_error{"GLFW window creation failed."};
		}
		
		glfwSetFramebufferSizeCallback(window.get(), framebuffer_callback);
		glfwMakeContextCurrent(window.get());
		glfwSwapInterval(0);	// Turn VSYNC off
		
		std::cout << "GLFW initialized successfully\n";


		// GLEW init
		glewExperimental = GL_TRUE;
		GLenum status = glewInit();

		if(status != GLEW_OK)
		{
			std::cerr << "GLEW failed to initialize with message: " << reinterpret_cast<const char*>(glewGetErrorString(status)) << '\n';
			throw std::runtime_error{"GLEW failed to initialize."};
		}

		std::cout << "GLEW initialized successfully\n";
	}


	void Application::execute()
	{
		setup_input();


		// Load mesh
		auto mesh = SoupMesh{"assets/cube.obj"};
		auto he_mesh = HalfEdgeMesh{mesh};
		mesh = static_cast<SoupMesh>(he_mesh);
		GLuint vao;
		GLuint vbo[2];
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		glGenBuffers(2, vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * mesh.get_positions().size(), mesh.get_positions().data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * mesh.get_indices().size(), mesh.get_indices().data(), GL_STATIC_DRAW);

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

		glEnable(GL_DEPTH_TEST);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		while(!glfwWindowShouldClose(window.get()))
		{
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// TODO:DO STUFF
			glDrawElements(GL_TRIANGLES, mesh.get_indices().size(), GL_UNSIGNED_INT, nullptr);
			
			glfwSwapBuffers(window.get());
			input.unstick();
			glfwPollEvents();
		}

		glDeleteVertexArrays(1, &vao);
		glDeleteBuffers(2, vbo);
		glDeleteShader(vs);
		glDeleteShader(fs);
		glDeleteProgram(program);
	}

	void Application::error_callback(int errorcode, const char* description)
	{
		std::cerr << "GLFW failed with error code " << errorcode << " and description: " << description;
		throw std::runtime_error{"GLFW encountered an error."};
	}

	void Application::framebuffer_callback(GLFWwindow* window, int width, int height)
	{
		glViewport(0, 0, width, height);
	}

	void Application::setup_input()
	{
		if(window)
		{
			glfwSetWindowUserPointer(window.get(), &input);

			auto key_callback = [] (GLFWwindow* window, int keycode, int /*scancode*/, int action, int /*mods*/) {
				auto input_ptr = static_cast<InputManager*>(glfwGetWindowUserPointer(window));
				if(input_ptr)
				{
					if(action == GLFW_PRESS)
						input_ptr->key_pressed(keycode);
					else if(action == GLFW_RELEASE)
						input_ptr->key_released(keycode);
				}
			};
			glfwSetKeyCallback(window.get(), key_callback);

			auto button_callback = [] (GLFWwindow* window, int buttoncode, int action, int /*modifier*/) {
				auto input_ptr = static_cast<InputManager*>(glfwGetWindowUserPointer(window));
				if(input_ptr)
				{
					if(action == GLFW_PRESS)
						input_ptr->key_pressed(buttoncode);
					else if(action == GLFW_RELEASE)
						input_ptr->key_released(buttoncode);
				}
			};
			glfwSetMouseButtonCallback(window.get(), button_callback);

			auto cursor_callback = [] (GLFWwindow* window, double x, double y) {
				auto input_ptr = static_cast<InputManager*>(glfwGetWindowUserPointer(window));
				if(input_ptr)
					input_ptr->cursor_moved(glm::vec2{static_cast<float>(x), static_cast<float>(y)});
			};
			glfwSetCursorPosCallback(window.get(), cursor_callback);

			auto scroll_callback = [] (GLFWwindow* window, double x, double y) {
				auto input_ptr = static_cast<InputManager*>(glfwGetWindowUserPointer(window));
				if(input_ptr)
					input_ptr->mouse_scrolled(glm::ivec2{static_cast<int>(x), static_cast<int>(y)});
			};
			glfwSetScrollCallback(window.get(), scroll_callback);
			
			auto focus_callback = [] (GLFWwindow* window, int /*focused*/) {
				auto input_ptr = static_cast<InputManager*>(glfwGetWindowUserPointer(window));
				if(input_ptr)
					input_ptr->ignore_cursor_once();
			};
			glfwSetWindowFocusCallback(window.get(), focus_callback);
		}
	}
}
