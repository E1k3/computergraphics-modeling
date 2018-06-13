#include "glutil.hpp"

#include <fstream>
#include <algorithm>
#include <iostream>

namespace cg
{
	void glutil::load_compile_shader(GLuint id, const std::vector<std::string>& file_paths)
	{
		std::vector<std::string> sources{};
		for(const auto& path : file_paths)
		{
			sources.emplace_back();
			std::ifstream ifs{path};
			std::copy(std::istreambuf_iterator<char>{ifs}, std::istreambuf_iterator<char>{},
					std::back_inserter(sources.back()));
			sources.back().push_back('\0');
		}
		std::vector<const char*> source_ptrs{};
		std::transform(sources.begin(), sources.end(), std::back_inserter(source_ptrs),
				[] (const auto& source) { return source.c_str(); });

		glShaderSource(id, static_cast<GLsizei>(source_ptrs.size()), source_ptrs.data(), nullptr);
		glCompileShader(id);

		GLint success{0};
		glGetShaderiv(id, GL_COMPILE_STATUS, &success);

		if(success != GL_TRUE)
		{
			int logsize{0};
			std::vector<char> log{};
			glGetShaderiv(id, GL_INFO_LOG_LENGTH, &logsize);
			log.resize(static_cast<size_t>(logsize));
			glGetShaderInfoLog(id, logsize, nullptr, log.data());

			std::cout << "GLUtil: Compilation of shader [";
			for(const auto& path : file_paths)
				std::cout << path << ' ';
			std::cout << "] failed with error: " << log.data() << '\n';
			throw std::runtime_error("Failed to load shader.");
		}
	}
}
