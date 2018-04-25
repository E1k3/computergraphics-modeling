#ifndef INPUTMANAGER_HPP
#define INPUTMANAGER_HPP

#include "glm/glm.hpp"

#include <unordered_map>

namespace cg
{
	class InputManager
	{
		public:
			void reset();

			void reset_keys();
			void reset_key(int keycode);
			void key_pressed(int keycode);
			void key_released(int keycode);
			bool get_key(int keycode);
			
			void reset_cursor();
			void ignore_cursor_once();
			void cursor_moved(glm::vec2 position);
			glm::vec2 get_cursor_position();
			glm::vec2 get_cursor_offset();
			
			void reset_scroll();
			void mouse_scrolled(glm::ivec2 offset);
			glm::ivec2 get_scroll_offset();

		private:
			bool ignore_cursor{true};

			std::unordered_map<int, bool> released_keys;
			std::unordered_map<int, bool> pressed_keys;

			glm::vec2 cursor_position;
			glm::vec2 cursor_offset;

			glm::ivec2 scroll_offset;
	};
}

#endif // INPUTMANAGER_HPP
