#include "soup_mesh.hpp"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include <iostream>


namespace cg
{
	SoupMesh::SoupMesh(const std::string& file_path)
	{
		auto importer = Assimp::Importer{};
		const aiScene* scene = importer.ReadFile(file_path, aiProcess_JoinIdenticalVertices);

		if(!scene)
		{
			std::cerr << "Assimp failed to read file " << file_path << " with message: " << importer.GetErrorString() << '\n';
			throw std::runtime_error{"Assimp failed to load file."};
		}
		if(!scene->HasMeshes() || scene->mMeshes[0]->mNumVertices == 0)
		{
			std::cerr << "Model " << file_path << " does not contain a mesh\n";
			throw std::runtime_error{"Model does not contain a mesh."};
		}

		// Load the first mesh
		auto& mesh = *scene->mMeshes[0];

		// Load positions
		if(mesh.HasPositions())
		{
			positions = std::vector<glm::vec3>{reinterpret_cast<glm::vec3*>(mesh.mVertices), reinterpret_cast<glm::vec3*>(mesh.mVertices) + mesh.mNumVertices};
		}

		// Load normals
		if(mesh.HasNormals())
		{
			normals = std::vector<glm::vec3>{reinterpret_cast<glm::vec3*>(mesh.mNormals), reinterpret_cast<glm::vec3*>(mesh.mNormals) + mesh.mNumVertices};
		}

		// Load the first channel of texture coordinates
		if(mesh.HasTextureCoords(0))
		{
			texture_coordinates.clear();
			texture_coordinates.reserve(mesh.mNumVertices);
			for(unsigned int i = 0; i < mesh.mNumVertices; ++i)
			{
				texture_coordinates.push_back(glm::vec2{mesh.mTextureCoords[0][i].x, mesh.mTextureCoords[0][i].y});
			}
		}

		// Load faces
		if(mesh.HasFaces())
		{
			faces.clear();
			faces.reserve(mesh.mNumFaces);
			for(unsigned int fi = 0; fi < mesh.mNumFaces; ++fi)
			{
				faces.push_back(std::vector<unsigned int>{mesh.mFaces[fi].mIndices, mesh.mFaces[fi].mIndices + mesh.mFaces[fi].mNumIndices});
			}
		}

		int num_ignored_primitives = 0;
		for(const auto& face : faces)
		{
			// Ignore lines and points
			if(face.size() < 3)
			{
				++num_ignored_primitives;
			}
			// Simply copy faces with 3 vertices
			else if(face.size() == 3)
			{
				indices.insert(indices.begin(), face.begin(), face.end());
			}
			// Triangulate faces with more than 3 vertices
			else if(face.size() > 3)
			{
				for(size_t i = 1; i < face.size()-1; ++i)
				{
					indices.push_back(face[0]);
					indices.push_back(face[i]);
					indices.push_back(face[i+1]);
				}
			}
		}
		
		if(num_ignored_primitives > 0)
		{
			std::cout << "While loading " << file_path << " indices of " << num_ignored_primitives << " primitives were discarded" << '\n';
		}
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

	const std::vector<unsigned int>& SoupMesh::get_indices() const
	{
		return indices;
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

	std::vector<unsigned int>& SoupMesh::get_indices()
	{
		return indices;
	}
}
