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
		for(const auto& face : soup.get_faces())
		{
			// Ignore faces with less than 3 vertices
			if(face.size() > 2)
			{
				for(size_t i = 0; i < face.size(); ++i)
				{
					//TODO
				}
			}
		}
	}
}
