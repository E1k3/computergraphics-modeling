#ifndef HALF_EDGE_MESH_HPP
#define HALF_EDGE_MESH_HPP

#include "soup_mesh.hpp"

#include "glm/glm.hpp"

#include <vector>
#include <list>
#include <string>

namespace cg
{
	class HalfEdgeMesh
	{
		public:
			struct HalfEdge;

			struct Face
			{
				HalfEdge* edge{nullptr};
			};

			struct Vertex
			{
				HalfEdge* edge{nullptr};
				glm::vec3 position{0.f};
				glm::vec3 normal{0.f};
				glm::vec2 texture_coordinate{0.f};
			};

			struct HalfEdge
			{
				HalfEdge* next_edge{nullptr};
				HalfEdge* companion_edge{nullptr};
				Face* face{nullptr};
				Vertex* next_vertex{nullptr};
			};

			static HalfEdgeMesh::HalfEdge* face_loop_next(HalfEdgeMesh::HalfEdge* current);
			static HalfEdgeMesh::HalfEdge* vertex_loop_next(HalfEdgeMesh::HalfEdge* current);

			explicit HalfEdgeMesh(const SoupMesh& soup);
			explicit operator SoupMesh() const;

		private:
			// HalfEdge structure
			std::list<HalfEdge> half_edges;
			std::vector<Face> faces;
			std::vector<Vertex> vertices;
	};
}

#endif // HALF_EDGE_MESH_HPP
