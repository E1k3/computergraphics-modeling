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
				HalfEdge* first = nullptr;
				for(size_t i = 0; i < face.size(); ++i)
				{
					size_t next_i = (i+1) % face.size();

					// Add clockwise halfedge
					half_edges.push_back(HalfEdge{
							nullptr,	// Next halfedge is not yet known
							nullptr,	// Companion halfedge is about to be added to the list
							nullptr,	// Face is not yet known
							&vertices[face[i]]});	// Clockwise vertex of the edge
					
					// Set next_edge of last to the current halfedge
					if(last)
					{
						last->next_edge = &half_edges.back();
					}
					else
					{
						first = &half_edges.back();
					}


					// Add counter-clockwise halfedge
					half_edges.push_back(HalfEdge{
							nullptr,	// Next halfedge is about to be added to the list
							&half_edges.back(), // Most recently added halfedge is companion
							&faces[fi],	// Counter-clockwise face is the current face
							&vertices[face[next_i]]});	// Counter-clockwise vertex of the edge
					// Set the companion edge that wasn't in the list before
					half_edges.back().companion_edge->companion_edge = &half_edges.back();

					last = &half_edges.back();
				}
				last->next_edge = first;
			}
		}

		// Merge duplicate HalfEdges
		constexpr auto merge_duplicates = [] (const HalfEdge& a, const HalfEdge& b) { 
			if(&a != &b && a.next_vertex == b.next_vertex && a.companion_edge->next_vertex == b.companion_edge->next_vertex)
			{
// TODO: find clean way to find dupes, merge them and then delete one of the two.
			}
		} -> bool;
		// Merging info to duplicate before removal
		// Might not be very clean, should probably change
		half_edges.remove_if([&] (const HalfEdge& he)
				{ 
					if(auto it = std::find(half_edges.begin(), half_edges.end(), [&] (const auto& dup) { return equal_not_same(he, dup); }) != half_edges.end()) 
					{
						
					}
				});
	}

	HalfEdgeMesh::operator SoupMesh() const
	{
		auto mesh = SoupMesh();

	}
}
