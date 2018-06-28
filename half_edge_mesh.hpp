#ifndef HALF_EDGE_MESH_HPP
#define HALF_EDGE_MESH_HPP

#include "soup_mesh.hpp"

#include "glm/glm.hpp"

#include <functional>
#include <memory>
#include <vector>
#include <unordered_map>
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

			/// Returns next half edge in a face loop around current->face or nullptr if one is reached.
			/// Throws invalid_argument when called with nullptr.
			static HalfEdge* face_loop_next(HalfEdge* current);

			/// Returns next half edge in a vertex loop around current->next_vertex or nullptr if one is reached.
			/// Throws invalid_argument when called with nullptr.
			static HalfEdge* vertex_loop_next(HalfEdge* current);

			/// Returns nullptr, if one is reached.
			/// Throws when called with nullptr.
			static HalfEdge* face_loop_prev(HalfEdge* current);

			/// Reverse vertex loop using a full face loop.
			/// Returns nullptr, if a boundary is reached.
			/// Throws when called with nullptr.
			static HalfEdge* vertex_loop_prev(HalfEdge* current);

			/// Returns the number of vertices the given face has.
			/// Throws invalid_argument when called with nullptr.
			static int vertex_count(Face* face);

			/// Uses multiple half edge collapses to simplify the mesh until the given proportion of edges is removed
			/// or the mesh cannot be further simplified.
			/// Returns the achieved proportion of removed edges.
			//float half_edge_simplify(float factor);

			explicit HalfEdgeMesh() = delete;
			explicit HalfEdgeMesh(const SoupMesh& soup);
			SoupMesh toSoupMesh() const;

		private:
			using EdgeKey = std::pair<Vertex*, Vertex*>;
			struct Hasher
			{
				size_t operator()(const EdgeKey& key) const
				{
					size_t first = std::hash<Vertex*>{}(key.first) << (sizeof(size_t) * 4);
					return first ^ std::hash<Vertex*>{}(key.second); 
				}
			};
			// HalfEdge structure
			std::unordered_map<EdgeKey, std::unique_ptr<HalfEdge>, Hasher> half_edges;
			std::vector<std::unique_ptr<Face>> faces;
			std::vector<std::unique_ptr<Vertex>> vertices;
	};
}

#endif // HALF_EDGE_MESH_HPP
