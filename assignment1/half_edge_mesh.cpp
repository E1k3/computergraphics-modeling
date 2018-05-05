#include "half_edge_mesh.hpp"

#include <iostream>

namespace cg
{
	HalfEdgeMesh::HalfEdgeMesh(const SoupMesh& soup)
	{
		if(soup.get_faces().empty() || soup.get_positions().empty())
		{
			std::cerr << "HalfEdge created from triangle soup that has no faces/vertices\n";
			throw std::runtime_error{"HalfEdge created from empty triangle soup."};
		}

		// Copy vertices
		for(const auto& position : soup.get_positions())
			vertices.push_back(Vertex{nullptr, position, glm::vec3{}, glm::vec2{}});

		// Copy faces
		for(const auto& face : soup.get_faces())
			faces.push_back(Face{nullptr});

		// Create HalfEdges
		for(size_t fi = 0; fi < soup.get_faces().size(); ++fi)
		{
			const auto& face = soup.get_faces()[fi];
			// Ignore faces with less than 3 vertices
			if(face.size() > 2)
			{
				HalfEdge* last = nullptr;
				for(size_t i = 0; i < face.size(); ++i)
				{
					size_t next_i = (i+1) % face.size();

					// Add left halfedge
					half_edges.push_back(HalfEdge{
							nullptr,	// Next halfedge is not yet known
							nullptr,	// Companion halfedge is about to be added to the list
							nullptr,	// Face is not yet known
							&vertices[face[i]]});	// Clockwise vertex of the edge
					
					// Set next_edge of last to the current halfedge
					last->next_edge = &half_edges.back();

					// Add right halfedge
					half_edges.push_back(HalfEdge{
							nullptr,	// Next halfedge is about to be added to the list
							&half_edges.back(), // Most recently added halfedge is companion
							&faces[fi],	// Counter-clockwise face is the current face
							&vertices[face[next_i]]});	// Counter-clockwise vertex of the edge
					// Set the companion edge that wasn't in the list before
					half_edges.back().companion_edge->companion_edge = &half_edges.back();

					last = &half_edges.back();
				}
				//half_edges.back().next_edge = &;
				// TODO: edges get added multiple times
				// find better algorithm
			}
		}
	}

	HalfEdgeMesh::operator SoupMesh() const
	{
		auto mesh = SoupMesh();

	}
}
