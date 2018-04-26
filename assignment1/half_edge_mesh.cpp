#include "half_edge_mesh.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/preprocess.h"

namespace cg
{
	HalfEdgeMesh::HalfEdgeMesh(std::string file_path)
	{
		auto importer = Assimp::Importer;
		const aiScene* scene = importer.ReadFile(file_path, aiProcess_JoinIdenticalVertices);

		if(!scene)
		{
			std::cerr << "Assimp failed to read file " << file_path << " with message: " << importer.GetErrorString() << '\n';
			throw std::runtime_error{"Assimp failed to load file."};
		}

	}
}
