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
			const std::vector<unsigned int>& get_indices() const;

			std::vector<glm::vec3>& get_positions();
			std::vector<glm::vec3>& get_normals();
			std::vector<glm::vec2>& get_texture_coordinates();

			std::vector<std::vector<unsigned int>>& get_faces();
			std::vector<unsigned int>& get_indices();

		private:
			void calculate_indices();


			std::vector<glm::vec3> positions;
			std::vector<glm::vec3> normals;
			std::vector<glm::vec2> texture_coordinates;

			std::vector<std::vector<unsigned int>> faces;
			std::vector<unsigned int> indices;
	};
}

#endif // SOUP_MESH_HPP
