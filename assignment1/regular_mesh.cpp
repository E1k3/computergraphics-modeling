#include "regular_mesh.hpp"

#include <iostream>


namespace cg
{
	RegularMesh::RegularMesh(size_t width, size_t height, const std::vector<glm::vec3> positions, const std::vector<glm::vec3>& normals, const std::vector<glm::vec2>& texture_coordinates)
		: width{width},
		  height{height},
		  positions{positions},
		  normals{normals},
		  texture_coordinates{texture_coordinates}
	{
		if(positions.empty() || positions.size() != width * height)
		{
			std::cerr << "Regular mesh construction missing positions or wrong dimensions\n";
			throw std::invalid_argument{"Regular mesh construction failed."};
		}
		else
		{
			if(!normals.empty() && positions.size() != normals.size())
			{
				std::cerr << "Regular mesh construction with differently sized position and normal collections\n";
				throw std::invalid_argument{"Regular mesh construction failed."};
			}
			if(!texture_coordinates.empty() && positions.size() != texture_coordinates.size())
			{
				std::cerr << "Regular mesh construction with differently sized position and texture_coordinate collections\n";
				throw std::invalid_argument{"Regular mesh construction failed."};
			}
		}
		this->normals.resize(positions.size());
		this->texture_coordinates.resize(positions.size());
	}

	std::vector<unsigned int> RegularMesh::calculate_indices() const
	{
		auto indices = std::vector<unsigned int>{};

		for(size_t row = 0; row < height - 1; ++row)
		{
			for(size_t col = 0; col < width - 1; ++col)
			{
				indices.push_back(row * width + col);
				indices.push_back((row + 1) * width + col);
				indices.push_back((row + 1) * width + col + 1);
				indices.push_back(row * width + col);
				indices.push_back((row + 1) * width + col + 1);
				indices.push_back(row * width + col + 1);
			}
		}
		return indices;
	}

	void RegularMesh::loop_subdivision()
	{
		auto new_positions = std::vector<glm::vec3>((width * 2 - 1) * (height * 2 - 1));
		auto new_normals = std::vector<glm::vec3>((width * 2 - 1) * (height * 2 - 1));
		auto new_texture_coordinates = std::vector<glm::vec2>((width * 2 - 1) * (height * 2 - 1));

		for(size_t row = 0; row < height; ++row)
		{
			for(size_t col = 0; col < width; ++col)
			{
				new_positions[row * (width * 2 - 1) + col * 2] = positions[row * width + col];
				new_normals[row * (width * 2 - 1) + col * 2] = normals[row * width + col];
				new_texture_coordinates[row * (width * 2 - 1) + col * 2] = texture_coordinates[row * width + col];
			}
		}
	}

	const std::vector<glm::vec3>& RegularMesh::get_positions() const
	{
		return positions;
	}

	const std::vector<glm::vec3>& RegularMesh::get_normals() const
	{
		return normals;
	}

	const std::vector<glm::vec2>& RegularMesh::get_texture_coordinates() const
	{
		return texture_coordinates;
	}

	std::vector<glm::vec3>& RegularMesh::get_positions()
	{
		return positions;
	}

	std::vector<glm::vec3>& RegularMesh::get_normals()
	{
		return normals;
	}

	std::vector<glm::vec2>& RegularMesh::get_texture_coordinates()
	{
		return texture_coordinates;
	}
}
