#ifndef HALF_EDGE_MESH_HPP
#define HALF_EDGE_MESH_HPP

#include "glm/glm.hpp"

#include <vector>
#include <string>

namespace cg
{
	class HalfEdgeMesh
	{
		public:
			struct HalfEdge
			{
				HalfEdge* next_edge;
				HalfEdge* companion_edge;
				Face* face;
				Vertex* next_vertex;
			};

			struct Face
			{
				HalfEdge* edge;
			};

			struct Vertex
			{
				HalfEdge* edge;
				glm::vec3* position;
				glm::vec3* normal;
				glm::vec2* texture_coordinate;
			};

			explicit HalfEdgeMesh(std::string file_path);

			const std::vector<glm::vec3>& get_positions() const;
			const std::vector<glm::vec3>& get_normals() const;
			const std::vector<glm::vec2>& get_texture_coordinates() const;
			const std::vector<int>& get_indices() const;

		private:
			std::vector<glm::vec3> positions;
			std::vector<glm::vec3> normals;
			std::vector<glm::vec2> texture_coordinates;
			std::vector<int> indices;

			std::vector<HalfEdge> half_edges;
			std::vector<Face> faces;
			std::vector<Vertex> vertices;
	};
}

#define HALF_EDGE_MESH_HPP