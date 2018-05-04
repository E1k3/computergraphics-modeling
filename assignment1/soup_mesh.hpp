#ifndef SOUP_MESH_HPP
#define SOUP_MESH_HPP

#include "glm/glm.hpp"

#include <string>
#include <vector>

namespace cg
{
	class SoupMesh
	{
		public:
			explicit SoupMesh(const std::string& file_path);

			const std::vector<glm::vec3>& get_positions() const;
			const std::vector<glm::vec3>& get_normals() const;
			const std::vector<glm::vec2>& get_texture_coordinates() const;

			const std::vector<std::vector<unsigned int>>& get_faces() const;
			const std::vector<unsigned int>& get_indices() const;

		private:
			std::vector<glm::vec3> positions;
			std::vector<glm::vec3> normals;
			std::vector<glm::vec2> texture_coordinates;

			std::vector<std::vector<unsigned int>> faces;
			std::vector<unsigned int> indices;
	};
}

#endif // SOUP_MESH_HPP
