#include "inputmanager.hpp"

namespace cg
{
	void InputManager::reset()
	{
		reset_keys();
		reset_cursor();
		reset_scroll();
	}

	void InputManager::reset_keys()
	{
		for(auto& key : pressed_keys)
			key.second = key.second && !released_keys[key.first];
		for(auto& key : released_keys)
			key.second = key.second || !pressed_keys[key.first];
	}

	void InputManager::reset_key(int keycode)
	{
		pressed_keys[keycode] = pressed_keys[keycode] && !released_keys[keycode];
		released_keys[keycode] = released_keys[keycode] || !pressed_keys[keycode];
	}

	void InputManager::key_pressed(int keycode)
	{
		pressed_keys[keycode] = true;
		released_keys[keycode] = false;
	}

	void InputManager::key_released(int keycode)
	{
		released_keys[keycode] = true;
	}

	bool InputManager::get_key(int keycode)
	{
		return pressed_keys[keycode];
	}

	void InputManager::reset_cursor()
	{
		cursor_offset = glm::vec2{0.f};
	}

	void InputManager::ignore_cursor_once()
	{
		ignore_cursor = true;
	}

	void InputManager::cursor_moved(glm::vec2 position)
	{
		if(!ignore_cursor)
			cursor_offset += position - cursor_position;
		cursor_position = position;

		ignore_cursor = false;
	}

	glm::vec2 InputManager::get_cursor_position()
	{
		return cursor_position;
	}

	glm::vec2 InputManager::get_cursor_offset()
	{
		return cursor_offset;
	}

	void InputManager::reset_scroll()
	{
		scroll_offset = glm::ivec2{0};
	}

	void InputManager::mouse_scrolled(glm::ivec2 offset)
	{
		scroll_offset += offset;
	}

	glm::ivec2 InputManager::get_scroll_offset()
	{
		return scroll_offset;
	}
}
