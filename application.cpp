#include "application.hpp"

#include "soup_mesh.hpp"
#include "half_edge_mesh.hpp"
#include "regular_mesh.hpp"
#include "glutil.hpp"

#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/type_ptr.hpp"

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
		
		std::cout << "Application: GLFW initialized successfully\n";


		// GLEW init
		glewExperimental = GL_TRUE;
		GLenum status = glewInit();

		if(status != GLEW_OK)
		{
			std::cerr << "GLEW failed to initialize with message: " << reinterpret_cast<const char*>(glewGetErrorString(status)) << '\n';
			throw std::runtime_error{"GLEW failed to initialize."};
		}

		std::cout << "Application: GLEW initialized successfully\n";
	}


	void Application::execute()
	{
		setup_input();


		// Load mesh
		//auto mesh = SoupMesh{"assets/al.obj"};
		//auto hemesh = HalfEdgeMesh{mesh};
		//mesh = static_cast<SoupMesh>(hemesh);
		//auto indices = mesh.calculate_indices();
		auto positions = std::vector<glm::vec3>(5*5);
		auto xys = std::array<float, 5>{-2.f, -1.f, 0.f, 1.f, 2.f};
		auto zs = std::array<float, 5>{1.f, 2.f, 5.f, 1.f, 3.f};
		for(size_t row = 0; row < 5; ++row)
			for(size_t col = 0; col < 5; ++col)
				positions[row * 5 + col] = glm::vec3{xys[row], xys[col], zs[row]};
		auto mesh = RegularMesh(5, 5, positions, {}, {});
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
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		while(!glfwWindowShouldClose(window.get()))
		{
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// TODO:DO STUFF
			if(input.get_key(GLFW_KEY_1))
			{
				input.key_released(GLFW_KEY_1);
				mesh.loop_subdivision();
				indices = mesh.calculate_indices();

				glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
				glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * mesh.get_positions().size(), mesh.get_positions().data(), GL_DYNAMIC_DRAW);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), indices.data(), GL_DYNAMIC_DRAW);
			}
			else if(input.get_key(GLFW_KEY_2))
			{
				input.key_released(GLFW_KEY_2);
				mesh.catmull_clark_subdivision();
				indices = mesh.calculate_indices();

				glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
				glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * mesh.get_positions().size(), mesh.get_positions().data(), GL_DYNAMIC_DRAW);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), indices.data(), GL_DYNAMIC_DRAW);
			}
			else if(input.get_key(GLFW_KEY_3))
			{
				input.key_released(GLFW_KEY_3);
				mesh.catmull_clark_subdivision_sharp_bounds();
				indices = mesh.calculate_indices();

				glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
				glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * mesh.get_positions().size(), mesh.get_positions().data(), GL_DYNAMIC_DRAW);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), indices.data(), GL_DYNAMIC_DRAW);
			}

			//mvp = glm::rotate(mvp, input.get_cursor_offset().x * sensitivity, glm::vec3{0.f, 0.f, 1.f});
			mvp = glm::rotate(static_cast<float>(glfwGetTime()) * sensitivity, glm::vec3{0.f, 0.2f, 1.f});
			glUniformMatrix4fv(mvp_uniform, 1, GL_FALSE, value_ptr(mvp));
			glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);
			
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

	void Application::framebuffer_callback(GLFWwindow* /*window*/, int width, int height)
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