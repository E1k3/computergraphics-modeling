#ifndef HALF_EDGE_MESH_HPP
#define HALF_EDGE_MESH_HPP

#include "soup_mesh.hpp"

#include "glm/glm.hpp"

#include <vector>
#include <string>

namespace cg
{
	class HalfEdgeMesh
	{
		public:
			struct HalfEdge;

			struct Face
			{
				HalfEdge* edge;
			};

			struct Vertex
			{
				HalfEdge* edge;
				glm::vec3* position{nullptr};
				glm::vec3* normal{nullptr};
				glm::vec2* texture_coordinate{nullptr};
			};

			struct HalfEdge
			{
				HalfEdge* next_edge;
				HalfEdge* companion_edge;
				Face* face;
				Vertex* next_vertex;
			};

			explicit HalfEdgeMesh(const SoupMesh& soup);

		private:
			// HalfEdge structure
			std::vector<HalfEdge> half_edges;
			std::vector<Face> faces;
			std::vector<Vertex> vertices;
	};
}

#endif // HALF_EDGE_MESH_HPP
