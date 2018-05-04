#ifndef GLUTIL_HPP
#define GLUTIL_HPP

#include "GL/glew.h"

#include <vector>
#include <string>

namespace cg::glutil
{
	void load_compile_shader(GLuint id, const std::vector<std::string>& file_paths);
}

#endif // GLUTIL_HPP
