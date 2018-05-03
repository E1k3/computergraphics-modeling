#include "half_edge_mesh.hpp"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include <iostream>

namespace cg
{
	HalfEdgeMesh::HalfEdgeMesh(std::string file_path)
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

		auto& mesh = *scene->mMeshes[0];
		if(mesh.HasPositions())
			positions = std::vector<glm::vec3>{reinterpret_cast<glm::vec3*>(mesh.mVertices), reinterpret_cast<glm::vec3*>(mesh.mVertices) + mesh.mNumVertices};

		if(mesh.HasNormals())
			normals = std::vector<glm::vec3>{reinterpret_cast<glm::vec3*>(mesh.mNormals), reinterpret_cast<glm::vec3*>(mesh.mNormals) + mesh.mNumVertices};

		if(mesh.HasTextureCoords(0))
		{
			texture_coordinates.clear();
			texture_coordinates.reserve(mesh.mNumVertices);
			for(unsigned int i = 0; i < mesh.mNumVertices; ++i)
				texture_coordinates.push_back(glm::vec2{mesh.mTextureCoords[0][i].x, mesh.mTextureCoords[0][i].y});
		}

		if(mesh.HasFaces())
		{
			indices.clear();
			for(unsigned int fi = 0; fi < mesh.mNumFaces; ++fi)
				for(unsigned int ii = 0; ii < mesh.mFaces[fi].mNumIndices; ++ii)
					indices.push_back(mesh.mFaces[fi].mIndices[ii]);
		}
	}

	const std::vector<glm::vec3>& HalfEdgeMesh::get_positions() const
	{
		return positions;
	}

	const std::vector<glm::vec3>& HalfEdgeMesh::get_normals() const
	{
		return normals;
	}

	const std::vector<glm::vec2>& HalfEdgeMesh::get_texture_coordinates() const
	{
		return texture_coordinates;
	}

	const std::vector<unsigned int>& HalfEdgeMesh::get_indices() const
	{
		return indices;
	}
}
