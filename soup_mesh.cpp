#include "soup_mesh.hpp"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include <iostream>


namespace cg
{
	SoupMesh::SoupMesh(const std::string& file_path)
	{
		std::cout << "SoupMesh: Started construction from model file\n";
		Assimp::Importer importer{};
		const aiScene* scene{importer.ReadFile(file_path, aiProcess_JoinIdenticalVertices)};

		if(!scene)
		{
			std::cerr << "SoupMesh: Assimp failed to read file " << file_path << " with message: " << importer.GetErrorString() << '\n';
			throw std::runtime_error{"SoupMesh: Assimp failed to load file."};
		}
		if(!scene->HasMeshes())
		{
			std::cerr << "SoupMesh: Model " << file_path << " does not contain a mesh\n";
			throw std::runtime_error{"SoupMesh: Model does not contain a mesh."};
		}

		// Reserve space for meshes and check if all have faces and positions
		unsigned int num_faces{0};
		unsigned int num_vertices{0};
		for(unsigned int mi{0}; mi < scene->mNumMeshes; ++mi)
		{
			if(scene->mMeshes[mi]->HasFaces())
				num_faces += scene->mMeshes[mi]->mNumFaces;
			else
			{
				std::cerr << "SoupMesh: Mesh" << mi << " in " << file_path << " does not have faces\n";
				throw std::invalid_argument{"SoupMesh: Construction from file failed."};
			}

			if(scene->mMeshes[mi]->HasPositions())
				num_faces += scene->mMeshes[mi]->mNumVertices;
			else
			{
				std::cerr << "SoupMesh: Mesh " << mi << " in " << file_path << " does not have positions\n";
				throw std::invalid_argument{"SoupMesh: Construction from file failed."};
			}
		}
		faces.reserve(num_faces);
		positions.reserve(num_vertices);
		normals.reserve(num_vertices);
		texture_coordinates.reserve(num_vertices);

		for(unsigned int mi{0}; mi < scene->mNumMeshes; ++mi)
		{
			auto& mesh{*scene->mMeshes[mi]};

			// Copy faces
			for(unsigned int fi{0}; fi < mesh.mNumFaces; ++fi)
			{
				faces.emplace_back(mesh.mFaces[fi].mIndices, mesh.mFaces[fi].mIndices + mesh.mFaces[fi].mNumIndices);
				// Offset indices to start at the current meshs vertices
				std::transform(faces.back().begin(), faces.back().end(), faces.back().begin(), [offset = positions.size()] (unsigned int& index) { return index + offset; });
			}

			// Copy positions
			std::copy(reinterpret_cast<glm::vec3*>(mesh.mVertices), reinterpret_cast<glm::vec3*>(mesh.mVertices) + mesh.mNumVertices, std::back_inserter(positions));
			
			// Copy normals
			if(mesh.HasNormals())
				std::copy(reinterpret_cast<glm::vec3*>(mesh.mNormals), reinterpret_cast<glm::vec3*>(mesh.mNormals) + mesh.mNumVertices, std::back_inserter(normals));
			// If the current mesh does not have any, fill with zeros
			else
				normals.resize(positions.size());

			// Copy the first channel of texture coordinates
			if(mesh.HasTextureCoords(0))
				for(unsigned int ti{0}; ti < mesh.mNumVertices; ++ti)
					texture_coordinates.emplace_back(mesh.mTextureCoords[0][ti].x, mesh.mTextureCoords[0][ti].y);
			// If the current mesh does not have any, fill with zeros
			else
				texture_coordinates.resize(positions.size());
		}

		// Remove degenerate faces
		auto old_size{faces.size()};
		constexpr auto is_degenerate{[] (std::vector<unsigned int> face) { std::sort(face.begin(), face.end()); return std::adjacent_find(face.begin(), face.end()) != face.end(); }};
		faces.erase(std::remove_if(faces.begin(), faces.end(), is_degenerate), faces.end());
		if(old_size != faces.size())
			std::cout << "SoupMesh: Removed " << old_size - faces.size() << " degenerate faces\n";

		std::cout << "SoupMesh: Successfully loaded and merged " << scene->mNumMeshes << " meshes with " << positions.size() << " vertices total from \"" << file_path << "\"\n"; 
	}

	SoupMesh::SoupMesh(const std::vector<glm::vec3> vertex_positions, const std::vector<glm::vec3>& vertex_normals, const std::vector<glm::vec2>& vertex_texture_coordinates, const std::vector<std::vector<unsigned int>>& mesh_faces)
		: positions{vertex_positions},
		  normals{vertex_normals},
		  texture_coordinates{vertex_texture_coordinates},
		  faces{mesh_faces}
	{
		if(positions.empty() || faces.empty())
		{
			std::cerr << "SoupMesh: Construction missing positions or faces\n";
			throw std::invalid_argument{"SoupMesh: Construction failed."};
		}
		
		if(!normals.empty() && positions.size() != normals.size())
		{
			std::cerr << "SoupMesh: Construction with differently sized position and normal collections\n";
			throw std::invalid_argument{"SoupMesh: Construction failed."};
		}
		if(!texture_coordinates.empty() && positions.size() != texture_coordinates.size())
		{
			std::cerr << "SoupMesh: Construction with differently sized position and texture_coordinate collections\n";
			throw std::invalid_argument{"SoupMesh: Construction failed."};
		}

		normals.resize(positions.size());
		texture_coordinates.resize(positions.size());
	}

	std::vector<unsigned int> SoupMesh::calculate_indices() const
	{
		std::vector<unsigned int> indices{};

		int num_ignored_primitives{0};
		for(const auto& face : faces)
		{
			// Ignore lines and points
			if(face.size() < 3)
				++num_ignored_primitives;
			// Copy faces with 3 vertices
			else if(face.size() == 3)
				indices.insert(indices.begin(), face.begin(), face.end());
			// Triangulate faces with more than 3 vertices
			else if(face.size() > 3)
			{
				for(size_t i{1}; i < face.size()-1; ++i)
				{
					indices.push_back(face[0]);
					indices.push_back(face[i]);
					indices.push_back(face[i+1]);
				}
			}
		}
		
		if(num_ignored_primitives > 0)
			std::cout << "SoupMesh: Indices of " << num_ignored_primitives << " primitives with less than 3 vertices were discarded" << '\n';
		
		return indices;
	}

	const std::vector<glm::vec3>& SoupMesh::get_positions() const
	{
		return positions;
	}

	const std::vector<glm::vec3>& SoupMesh::get_normals() const
	{
		return normals;
	}

	const std::vector<glm::vec2>& SoupMesh::get_texture_coordinates() const
	{
		return texture_coordinates;
	}

	const std::vector<std::vector<unsigned int>>& SoupMesh::get_faces() const
	{
		return faces;
	}

	std::vector<glm::vec3>& SoupMesh::get_positions()
	{
		return positions;
	}

	std::vector<glm::vec3>& SoupMesh::get_normals()
	{
		return normals;
	}

	std::vector<glm::vec2>& SoupMesh::get_texture_coordinates()
	{
		return texture_coordinates;
	}

	std::vector<std::vector<unsigned int>>& SoupMesh::get_faces()
	{
		return faces;
	}
}
