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
			explicit SoupMesh() = default;
			explicit SoupMesh(const std::string& file_path);
			explicit SoupMesh(const std::vector<glm::vec3> positions, const std::vector<glm::vec3>& normals, const std::vector<glm::vec2>& texture_coordinates, const std::vector<std::vector<unsigned int>>& faces);

			const std::vector<glm::vec3>& get_positions() const;
			const std::vector<glm::vec3>& get_normals() const;
			const std::vector<glm::vec2>& get_texture_coordinates() const;
			const std::vector<std::vector<unsigned int>>& get_faces() const;

			std::vector<glm::vec3>& get_positions();
			std::vector<glm::vec3>& get_normals();
			std::vector<glm::vec2>& get_texture_coordinates();
			std::vector<std::vector<unsigned int>>& get_faces();

			std::vector<unsigned int> calculate_indices() const;

		private:
			std::vector<glm::vec3> positions;
			std::vector<glm::vec3> normals;
			std::vector<glm::vec2> texture_coordinates;

			std::vector<std::vector<unsigned int>> faces;
	};
}

#endif // SOUP_MESH_HPP
