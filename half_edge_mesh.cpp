#include "half_edge_mesh.hpp"

#include <iostream>
#include <algorithm>

namespace cg
{
	HalfEdgeMesh::HalfEdgeMesh(const SoupMesh& soup)
	{
		std::cout << "HalfEdgeMesh: Started construction from SoupMesh\n";
		if(soup.get_faces().empty() || soup.get_positions().empty())
		{
			std::cerr << "HalfEdgeMesh: Constructed from triangle soup that has no faces or no vertices\n";
			throw std::runtime_error{"HalfEdgeMesh: Constructed from empty triangle soup."};
		}

		// Copy vertices
		vertices.resize(soup.get_positions().size());
		for(size_t i{0}; i < vertices.size(); ++i)
		{
			vertices[i] = std::make_unique<Vertex>();

			vertices[i]->position = soup.get_positions()[i];
			if(!soup.get_normals().empty())
				vertices[i]->normal = soup.get_normals()[i];
			if(!soup.get_texture_coordinates().empty())
				vertices[i]->texture_coordinate = soup.get_texture_coordinates()[i];
		}

		// Create faces
		faces.resize(soup.get_faces().size());
		std::generate(faces.begin(), faces.end(), [] () { return std::make_unique<Face>(); });

		// Create HalfEdges
		for(size_t fi{0}; fi < soup.get_faces().size(); ++fi)
		{
			const auto& face{soup.get_faces()[fi]};
			// Ignore faces with less than 3 vertices
			if(face.size() >= 3)
			{
				EdgeKey last{nullptr, nullptr};
				for(size_t i{0}; i < face.size(); ++i)
				{
					Vertex* cw_vertex{vertices[face[i]].get()};
					Vertex* ccw_vertex{vertices[face[(i+1) % face.size()]].get()};
					EdgeKey cw_edge_key{cw_vertex, ccw_vertex};
					EdgeKey ccw_edge_key{ccw_vertex, cw_vertex};

					// Create HalfEdges if they don't exist already
					half_edges.insert({cw_edge_key, std::make_unique<HalfEdge>()});
					half_edges.insert({ccw_edge_key, std::make_unique<HalfEdge>()});
					
					// Set clockwise halfedge refs
					half_edges[cw_edge_key]->next_vertex = cw_vertex;
					half_edges[cw_edge_key]->companion_edge = half_edges[ccw_edge_key].get();
					
					// Set current edge as last.next_edge
					if(last.first && last.second)
						half_edges[last]->next_edge = half_edges[cw_edge_key].get();

					// Set counter-clockwise halfedge refs
					half_edges[ccw_edge_key]->companion_edge = half_edges[cw_edge_key].get();
					half_edges[ccw_edge_key]->face = faces[fi].get();
					half_edges[ccw_edge_key]->next_vertex = ccw_vertex;

					last = ccw_edge_key;
				}
				// Close the loop
				half_edges[last]->next_edge = half_edges[EdgeKey{vertices[face[0]].get(), vertices[face[1]].get()}].get();
			}
		}

		// Set reference edge for each vertex and each face
		for(auto& p : half_edges)
		{
			auto& he{p.second};

			if(!he->next_vertex->edge)
				he->next_vertex->edge = he.get();
			if(he->face && !he->face->edge)
				he->face->edge = he.get();
		}

		// Sort vertices for fast index calculation
		std::sort(vertices.begin(), vertices.end(), [] (const auto& v1, const auto& v2) { return v1.get() < v2.get(); });

		std::cout << "HalfEdgeMesh: Successfully created HalfEdgeMesh from SoupMesh with " << faces.size() << " faces, " << half_edges.size() << " half edges and " << vertices.size() << " vertices\n"; 
	}

	SoupMesh HalfEdgeMesh::toSoupMesh() const
	{
		std::cout << "HalfEdgeMesh: Started HalfEdgeMesh to SoupMesh conversion\n";
		std::vector<glm::vec3> soup_positions{vertices.size()};
		std::vector<glm::vec3> soup_normals{vertices.size()};
		std::vector<glm::vec2> soup_texture_coordinates{vertices.size()};
		std::vector<std::vector<unsigned int>> soup_faces{};

		std::transform(vertices.begin(), vertices.end(), soup_positions.begin(), [] (const auto& v) { return v->position; });
		std::transform(vertices.begin(), vertices.end(), soup_normals.begin(), [] (const auto& v) { return v->normal; });
		std::transform(vertices.begin(), vertices.end(), soup_texture_coordinates.begin(), [] (const auto& v) { return v->texture_coordinate; });

		soup_faces.reserve(faces.size());
		for(const auto& face : faces)
		{
			soup_faces.push_back(std::vector<unsigned int>{});
			HalfEdge* current{face->edge};

			do {
				// Search for the index of the next vector using fast binary search
				auto it{std::lower_bound(vertices.begin(), vertices.end(), current->next_vertex, [] (const auto& v1, const Vertex* v2) { return v1.get() < v2; })};
				if(it->get() != current->next_vertex)
				{
					std::cerr << "HalfEdgeMesh: Unexpected Error during conversion. Next vertex not found\n";
					throw std::runtime_error{"HalfEdgeMesh: Conversion to SoupMesh failed."};
				}

				unsigned int index{static_cast<unsigned int>(std::distance(vertices.begin(), it))};
				soup_faces.back().push_back(index);
				current = face_loop_next(current);
			} while(current != face->edge);
		}

		std::cout << "HalfEdgeMesh: Successfully converted HalfEdgeMesh to SoupMesh with " << soup_faces.size() << " faces and " << soup_positions.size() << " vertices\n";

		return SoupMesh(soup_positions, soup_normals, soup_texture_coordinates, soup_faces);
	}

	HalfEdgeMesh::HalfEdge* HalfEdgeMesh::face_loop_next(HalfEdgeMesh::HalfEdge* current)
	{
		if(current && current->next_edge && current->next_edge->companion_edge)
			return current->next_edge->companion_edge;
		throw std::invalid_argument("Faceloop reached nullptr.");
	}

	HalfEdgeMesh::HalfEdge* HalfEdgeMesh::vertex_loop_next(HalfEdgeMesh::HalfEdge* current)
	{
		if(current && current->next_edge)
			return current->next_edge;
		throw std::invalid_argument("Vertexloop reached nullptr.");
	}
}
