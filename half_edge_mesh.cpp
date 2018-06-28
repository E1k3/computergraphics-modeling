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

		// Sort vertices and faces for fast search for pointers
		std::sort(vertices.begin(), vertices.end());
		std::sort(faces.begin(), faces.end());

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
			} while(current && current != face->edge);

			if(!current)
				throw std::runtime_error{"HalfEdgeMesh: Faceloop reached nullptr during conversion to SoupMesh."};
		}

		std::cout << "HalfEdgeMesh: Successfully converted HalfEdgeMesh to SoupMesh with " << soup_faces.size() << " faces and " << soup_positions.size() << " vertices\n";

		return SoupMesh(soup_positions, soup_normals, soup_texture_coordinates, soup_faces);
	}

	HalfEdgeMesh::HalfEdge* HalfEdgeMesh::face_loop_next(HalfEdgeMesh::HalfEdge* current)
	{
		if(!current)
			throw std::invalid_argument("HalfEdgeMesh: Faceloop called with nullptr.");

		if(current->next_edge)
			return current->next_edge->companion_edge;

		return nullptr;
	}

	HalfEdgeMesh::HalfEdge* HalfEdgeMesh::vertex_loop_next(HalfEdgeMesh::HalfEdge* current)
	{
		if(!current)
			throw std::invalid_argument("HalfEdgeMesh: Vertexloop called with nullptr.");

		return current->next_edge;
	}

	HalfEdgeMesh::HalfEdge* HalfEdgeMesh::face_loop_prev(HalfEdgeMesh::HalfEdge* current)
	{
		if(!current)
			throw std::invalid_argument{"HalfEdgeMesh: Reverse vertex loop called with nullptr."};

		const HalfEdge* start{current};
		HalfEdge* next;
		while(current && (next = face_loop_next(current)) != start)
			current = next;

		return current;
	}

	HalfEdgeMesh::HalfEdge* HalfEdgeMesh::vertex_loop_prev(HalfEdgeMesh::HalfEdge* current)
	{
		if(!current)
			throw std::invalid_argument{"HalfEdgeMesh: Reverse vertex loop called with nullptr."};

		const HalfEdge* start{current};
		current = start->companion_edge;
		while(current && (current = face_loop_next(current)) && vertex_loop_next(current) != start)
		{}

		return current;
	}

	int HalfEdgeMesh::vertex_count(HalfEdgeMesh::Face* face)
	{
		if(!face)
			throw std::invalid_argument{"HalfEdgeMesh: Vertex count called with nullptr."};

		int count{0};
		HalfEdge* current{face->edge};
		while(current && (current = face_loop_next(current)) != face->edge)
			++count;

		if(!current)
			throw std::runtime_error{"HalfEdgeMesh: Face loop in vertex count reached nullptr."};

		return count;
	}

	//float HalfEdgeMesh::half_edge_simplify(float factor)
	//{
	//	if(factor <= 0.f || factor >= 1.f)
	//		throw std::invalid_argument{"HalfEdgeMesh: Half edge simplify called with factor outside (0, 1)."};

	//	auto has_boundary{[] (Vertex* v) {
	//		HalfEdge* current = v->edge;
	//		while(current && current != v->edge)
	//			current = vertex_loop_next(current);
	//		return current == nullptr;
	//	}};

	//	std::unordered_map<HalfEdge*, float> edges{};
	//	for(const auto& he : half_edges)
	//	{
	//		// Add at most one half edge per edge
	//		// Do not add half edges that have a boundary at their vertex
	//		if(!edges.count(he.second->companion_edge) &&
	//				!has_boundary(he.second->next_vertex))
	//		{
	//			edges[he.second.get()] = glm::distance(he.second->next_vertex->position, he.second->companion_edge->next_vertex->position);
	//		}
	//	}
	//	
	//	const auto original_edge_count{half_edges.size() / 2};

	//	while(edges.size() > 0 && original_edge_count * factor < half_edges.size() / 2)
	//	{
	//		HalfEdge* edge{std::min_element(edges.begin(), edges.end(), 
	//				[] (const auto& a, const auto& b) { return a.second < b.second; })->first};

	//		// Loop around collapsing vertex and set vertex pointers to the opposite vertex
	//		{
	//			HalfEdge* current{vertex_loop_next(edge)};
	//			HalfEdge* next;
	//			while(current && (next = vertex_loop_next(current)) != edge)
	//			{
	//				current->next_vertex = edge->companion_edge->next_vertex;
	//				current = next;
	//			}
	//			// If a full vertex loop was not possible, don't even try
	//			if(!current)
	//				throw std::runtime_error{"HalfEdgeMesh: Half edge collapse called with boundary facing half edge."};
	//		}

	//		if(edge->face)
	//		{
	//			HalfEdge* fl_prev{face_loop_prev(edge)};
	//			HalfEdge* fl_next{face_loop_next(edge)};
	//			if(!fl_prev || !fl_next)
	//				throw std::runtime_error{"HalfEdgeMesh: Face loop in edge collapse reached nullptr."};

	//			// Leap over the collapsing edge
	//			fl_prev->next_edge = fl_next;
	//			// Set edge reference of the face to a non collapsing edge
	//			if(edge->face->edge == edge)
	//				edge->face->edge = fl_next;

	//			//// Check if face will be degenerate after collapse.TODO
	//			//if(false && vertex_count(edge->face) == 3)
	//			//{
	//			//	HalfEdge* start{face_loop_next(edge)};
	//			//	HalfEdge* current{start};
	//			//	HalfEdge* next;
	//			//	while(current && (next = vertex_loop_next(current)) != start)
	//			//		current = next;
	//			//	if(!current)
	//			//		throw std::runtime_error{"HalfEdgeMesh: Vertex loop in mesh simplify reached nullptr."};
	//			//	current->next_edge = vertex_loop_next(start);
	//			//	edges.erase(start);
	//			//}

	//			//if(vertex_count(edge->face) < 3)
	//			//	throw std::runtime_error{"HalfEdgeMesh: Encountered degenerate mesh in half edge simplify."};
	//		}

	//		// Find out if the counter clockwise face exists and if it is going to be degenerate after the collapse
	//		if(edge->companion_edge->face)
	//		{
	//			HalfEdge* fl_prev{face_loop_prev(edge->companion_edge)};
	//			HalfEdge* fl_next{face_loop_next(edge->companion_edge)};
	//			if(!fl_prev || !fl_next)
	//				throw std::runtime_error{"HalfEdgeMesh: Face loop in edge collapse reached nullptr."};

	//			// Leap over the collapsing edge
	//			fl_prev->next_edge = fl_next;
	//			// Set edge reference of the face to a non collapsing edge
	//			if(edge->companion_edge->face->edge == edge->companion_edge)
	//				edge->companion_edge->face->edge = fl_next;

	//			//// Check if face will be degenerate after collapse.TODO
	//			//if(vertex_count(edge->companion_edge->face) == 3)
	//			//{
	//			//}

	//			//if(vertex_count(edge->companion_edge->face) < 3)
	//			//	throw std::runtime_error{"HalfEdgeMesh: Encountered degenerate mesh in half edge simplify."};
	//		}

	//		// Save modified length of adjacent edges
	//		HalfEdge* current = edge->companion_edge;
	//		while(current && current != edge->companion_edge)
	//		{
	//			if(edges.count(current))
	//				edges[current] = glm::distance(current->next_vertex->position, current->companion_edge->next_vertex->position);
	//			else if(edges.count(current->companion_edge))
	//				edges[current->companion_edge] = glm::distance(current->next_vertex->position, current->companion_edge->next_vertex->position);
	//			current = vertex_loop_next(current);
	//		}

	//		// Remove the half edges and the vertex
	//		edges.erase(edge);
	//		half_edges.erase(EdgeKey{edge->next_vertex, edge->companion_edge->next_vertex});
	//		half_edges.erase(EdgeKey{edge->companion_edge->next_vertex, edge->next_vertex});
	//		vertices.erase(std::lower_bound(vertices.begin(), vertices.end(), edge->next_vertex, [] (const auto& a_uptr, const Vertex* b_rptr) { return a_uptr.get() < b_rptr; }));
	//		std::cout << "Removed edge, " << static_cast<float>(half_edges.size() / 2) / original_edge_count << '/' << factor << " done\n";
	//	}

	//	return static_cast<float>(half_edges.size()/2) / original_edge_count;
	//}
}
