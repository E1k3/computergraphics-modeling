#ifndef REGULAR_MESH_HPP
#define REGULAR_MESH_HPP

#include "glm/glm.hpp"

#include <string>
#include <vector>

namespace cg
{
	class RegularMesh
	{
		public:
			RegularMesh() = delete;
			explicit RegularMesh(size_t width, size_t height, const std::vector<glm::vec3> positions, const std::vector<glm::vec3>& normals, const std::vector<glm::vec2>& texture_coordinates);

			size_t get_width() const;
			size_t get_height() const;

			const std::vector<glm::vec3>& get_positions() const;
			const std::vector<glm::vec3>& get_normals() const;
			const std::vector<glm::vec2>& get_texture_coordinates() const;

			std::vector<glm::vec3>& get_positions();
			std::vector<glm::vec3>& get_normals();
			std::vector<glm::vec2>& get_texture_coordinates();

			std::vector<unsigned int> calculate_indices() const;
			
			void loop_subdivision();

		private:
			size_t width{0};
			size_t height{0};
			std::vector<glm::vec3> positions;
			std::vector<glm::vec3> normals;
			std::vector<glm::vec2> texture_coordinates;
	};
}

#endif // REGULAR_MESH_HPP
