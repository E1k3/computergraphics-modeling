#include "half_edge_mesh.hpp"

#include <iostream>
#include <algorithm>

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
		vertices.resize(std::max({soup.get_positions().size(), soup.get_normals().size(), soup.get_texture_coordinates().size()}));
		for(size_t i = 0; i < vertices.size(); ++i)
		{
				vertices[i].position = soup.get_positions()[i];
			if(!soup.get_normals().empty())
				vertices[i].normal = soup.get_normals()[i];
			if(!soup.get_texture_coordinates().empty())
				vertices[i].texture_coordinate = soup.get_texture_coordinates()[i];
		}

		// Create faces
		faces.resize(soup.get_faces().size());

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
					half_edges.push_back(HalfEdge{nullptr, nullptr, nullptr, &vertices[face[i]]});
					
					// Set current edge as last.next_edge
					if(last)
						last->next_edge = &half_edges.back();
					// Save first halfedge to close the loop
					else
						first = &half_edges.back();

					// Add counter-clockwise halfedge
					half_edges.push_back(HalfEdge{nullptr, &half_edges.back(), &faces[fi], &vertices[face[next_i]]});

					// Set the companion edge of clockwise halfedge
					half_edges.back().companion_edge->companion_edge = &half_edges.back();

					last = &half_edges.back();
				}
				// Close the loop
				last->next_edge = first;
			}
		}

		// Merge duplicate HalfEdges
		const auto merge = [this] (const HalfEdge& current)
		{
			// Find different half edge connecting the same vertices
			auto duplicate = std::find_if(half_edges.begin(), half_edges.end(), [&current] (const HalfEdge& other) { return &current != &other && current.next_vertex == other.next_vertex && current.companion_edge->next_vertex == other.companion_edge->next_vertex; });
			// If duplicate is found
			if(duplicate != half_edges.end())
			{
				// Merge face and next_edge into duplicate and mark this half_edge for deletion
				if(!duplicate->face)
					duplicate->face = current.face;

				if(!duplicate->next_edge)
					duplicate->next_edge = current.next_edge;

				// Pointers to current should now point to duplicate
				for(HalfEdge& h : half_edges)
				{
					if(h.next_edge == &current)
						h.next_edge = &(*duplicate);
					if(h.companion_edge == &current)
						h.companion_edge = &(*duplicate);
				}
				return true;
			}
			return false;
		};
		auto current = half_edges.begin();
		while(current != half_edges.end())
		{
			if(merge(*current))
				half_edges.erase(current++);
			else
				++current;
		}

		// Set reference edge for each vertex and each face
		for(auto& he : half_edges)
		{
			if(!he.next_vertex->edge)
				he.next_vertex->edge = &he;
			if(he.face && !he.face->edge)
				he.face->edge = &he;
		}
	}

	HalfEdgeMesh::operator SoupMesh() const
	{
		std::vector<glm::vec3> soup_positions{vertices.size()};
		std::vector<glm::vec3> soup_normals{vertices.size()};
		std::vector<glm::vec2> soup_texture_coordinates{vertices.size()};
		std::vector<std::vector<unsigned int>> soup_faces{};

		std::transform(vertices.begin(), vertices.end(), soup_positions.begin(), [] (const auto& v) { return v.position; });
		std::transform(vertices.begin(), vertices.end(), soup_normals.begin(), [] (const auto& v) { return v.normal; });
		std::transform(vertices.begin(), vertices.end(), soup_texture_coordinates.begin(), [] (const auto& v) { return v.texture_coordinate; });

		for(const auto& face : faces)
		{
			soup_faces.push_back(std::vector<unsigned int>{});
			HalfEdge* current = face.edge;

			do {
				auto index = static_cast<unsigned int>(std::distance<const Vertex*>(vertices.data(), current->next_vertex));
				soup_faces.back().push_back(index);
				current = face_loop_next(current);
			} while(current != face.edge);
		}
		for(const auto& face : soup_faces)
		{
			for(const auto& index : face)
				std::cout << index << ' ';
			std::cout << "::\n";
		}

		return SoupMesh(soup_positions, soup_normals, soup_texture_coordinates, soup_faces);
	}

	HalfEdgeMesh::HalfEdge* HalfEdgeMesh::face_loop_next(HalfEdgeMesh::HalfEdge* current)
	{
		if(current && current->next_edge)
			return current->next_edge->companion_edge;
		throw std::invalid_argument("Faceloop next was given nullptr.");
	}

	HalfEdgeMesh::HalfEdge* HalfEdgeMesh::vertex_loop_next(HalfEdgeMesh::HalfEdge* current)
	{
		if(current)
			return current->next_edge;
		throw std::invalid_argument("Vertexloop next was given nullptr.");
	}
}
