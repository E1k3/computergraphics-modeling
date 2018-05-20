#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include "inputmanager.hpp"

#include "GL/glew.h"
#include "GLFW/glfw3.h"

#include <string>
#include <memory>
#include <functional>

namespace cg
{
	class Application
	{
		// Unique pointer that destroys the window and terminates GLFW using RAII.
		using WindowPointer = std::unique_ptr<GLFWwindow, std::function<void(GLFWwindow*)>>;

		public:
			explicit Application(std::string title, int window_width = 640, int window_height = 480);

			GLFWwindow* get_window() const;
			void set_input(InputManager* input);


		private:
			[[noreturn]] static void error_callback(int errorcode, const char* description);
			static void framebuffer_callback(GLFWwindow* window, int width, int height);

			WindowPointer window;
	};
}

#endif // APPLICATION_HPP
