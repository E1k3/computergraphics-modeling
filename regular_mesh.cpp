#include "regular_mesh.hpp"

#include <iostream>
#include <numeric>


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
		if(width == 1 || height == 1)
		{
			std::cout << "RegularMesh: Loop subdivision does not work on meshes smaller than 2x2\n";
			throw std::runtime_error{"Regular mesh loop subdivision failed."};
		}
		auto new_width = width * 2 - 1;
		auto new_positions = std::vector<glm::vec3>(new_width * (height * 2 - 1));
		auto new_normals = std::vector<glm::vec3>(new_width * (height * 2 - 1));
		auto new_texture_coordinates = std::vector<glm::vec2>(new_width * (height * 2 - 1));

		for(size_t row = 0; row < height; ++row)
		{
			for(size_t col = 0; col < width; ++col)
			{
				// Odd vertices
				// Right
				if((row == 0 || row + 1 == height) && col + 1 < width)
				{
					new_positions[row * 2 * new_width + col * 2 + 1] = (positions[row * width + col]
							+ positions[row * width + col + 1]) / 2.f;

					new_normals[row * 2 * new_width + col * 2 + 1] = (normals[row * width + col]
							+ normals[row * width + col + 1]) / 2.f;

					new_texture_coordinates[row * 2 * new_width + col * 2 + 1] = (texture_coordinates[row * width + col]
							+ texture_coordinates[row * width + col + 1]) / 2.f;
				}
				else if(col + 1 < width)
				{
					new_positions[row * 2 * new_width + col * 2 + 1] = (positions[row * width + col] * 3.f
							+ positions[row * width + col + 1] * 3.f
							+ positions[(row - 1) * width + col]
							+ positions[(row + 1) * width + col]) / 8.f;

					new_normals[row * 2 * new_width + col * 2 + 1] = (normals[row * width + col] * 3.f
							+ normals[row * width + col + 1] * 3.f
							+ normals[(row - 1) * width + col]
							+ normals[(row + 1) * width + col]) / 8.f;

					new_texture_coordinates[row * 2 * new_width + col * 2 + 1] = (texture_coordinates[row * width + col] * 3.f
							+ texture_coordinates[row * width + col + 1] * 3.f
							+ texture_coordinates[(row - 1) * width + col]
							+ texture_coordinates[(row + 1) * width + col]) / 8.f;
				}

				// Down
				if((col == 0 || col + 1 == width) && row + 1 < height)
				{
					new_positions[(row * 2 + 1) * new_width + col * 2] = (positions[row * width + col]
							+ positions[(row + 1) * width + col]) / 2.f;
					
					new_normals[(row * 2 + 1) * new_width + col * 2] = (normals[row * width + col]
							+ normals[(row + 1) * width + col]) / 2.f;
					
					new_texture_coordinates[(row * 2 + 1) * new_width + col * 2] = (texture_coordinates[row * width + col]
							+ texture_coordinates[(row + 1) * width + col]) / 2.f;
				}
				else if(row + 1 < height)
				{
					new_positions[(row * 2 + 1) * new_width + col * 2] = (positions[row * width + col] * 3.f
							+ positions[(row + 1) * width + col] * 3.f
							+ positions[row * width + col - 1]
							+ positions[(row + 1) * width + col + 1]) / 8.f;
					
					new_normals[(row * 2 + 1) * new_width + col * 2] = (normals[row * width + col] * 3.f
							+ normals[(row + 1) * width + col] * 3.f
							+ normals[row * width + col - 1]
							+ normals[(row + 1) * width + col + 1]) / 8.f;
					
					new_texture_coordinates[(row * 2 + 1) * new_width + col * 2] = (texture_coordinates[row * width + col] * 3.f
							+ texture_coordinates[(row + 1) * width + col] * 3.f
							+ texture_coordinates[row * width + col - 1]
							+ texture_coordinates[(row + 1) * width + col + 1]) / 8.f;
				}

				// Diagonal
				if(row + 1 < height && col + 1 < width)
				{
					new_positions[(row * 2 + 1) * new_width + col * 2 + 1] = (positions[row * width + col] * 3.f
							+ positions[(row + 1) * width + col + 1] * 3.f
							+ positions[row * width + col + 1]
							+ positions[(row + 1) * width + col]) / 8.f;

					new_normals[(row * 2 + 1) * new_width + col * 2 + 1] = (normals[row * width + col] * 3.f
							+ normals[(row + 1) * width + col + 1] * 3.f
							+ normals[row * width + col + 1]
							+ normals[(row + 1) * width + col]) / 8.f;

					new_texture_coordinates[(row * 2 + 1) * new_width + col * 2 + 1] = (texture_coordinates[row * width + col] * 3.f
							+ texture_coordinates[(row + 1) * width + col + 1] * 3.f
							+ texture_coordinates[row * width + col + 1]
							+ texture_coordinates[(row + 1) * width + col]) / 8.f;
				}

				// Even vertices
				// Horizontal boundary vertex
				if((row == 0 || row + 1 == height) && col > 0 && col + 1 < width)
				{
					new_positions[row * 2 * new_width + col * 2] = (positions[row * width + col] * 6.f
							+ new_positions[row * 2 * new_width + col * 2 - 1]
							+ new_positions[row * 2 * new_width + col * 2 + 1]) / 8.f;

					new_normals[row * 2 * new_width + col * 2] = (normals[row * width + col] * 6.f
							+ new_normals[row * 2 * new_width + col * 2 - 1]
							+ new_normals[row * 2 * new_width + col * 2 + 1]) / 8.f;

					new_texture_coordinates[row * 2 * new_width + col * 2] = (texture_coordinates[row * width + col] * 6.f
							+ new_texture_coordinates[row * 2 * new_width + col * 2 - 1]
							+ new_texture_coordinates[row * 2 * new_width + col * 2 + 1]) / 8.f;
				}
				// Vertical boundary vertex
				else if((col == 0 || col + 1 == width) && row > 0 && row + 1 < height)
				{
					new_positions[row * 2 * new_width + col * 2] = (positions[row * width + col] * 6.f
							+ new_positions[(row * 2 - 1) * new_width + col * 2]
							+ new_positions[(row * 2 + 1) * new_width + col * 2]) / 8.f;

					new_normals[row * 2 * new_width + col * 2] = (normals[row * width + col] * 6.f
							+ new_normals[(row * 2 - 1) * new_width + col * 2]
							+ new_normals[(row * 2 + 1) * new_width + col * 2]) / 8.f;

					new_texture_coordinates[row * 2 * new_width + col * 2] = (texture_coordinates[row * width + col] * 6.f
							+ new_texture_coordinates[(row * 2 - 1) * new_width + col * 2]
							+ new_texture_coordinates[(row * 2 + 1) * new_width + col * 2]) / 8.f;
				}
				// Corner vertex
				else if(row == 0 || row + 1 == height || col == 0 || col + 1 == width)
				{
					new_positions[row * 2 * new_width + col * 2] = positions[row * width + col];
					new_normals[row * 2 * new_width + col * 2] = normals[row * width + col];
					new_texture_coordinates[row * 2 * new_width + col * 2] = texture_coordinates[row * width + col];
				}
				// Regular inside vertex
				else
				{
					new_positions[row * 2 * new_width + col * 2] = (positions[row * width + col] * 5.f
							+ new_positions[(row * 2 - 1) * new_width + col * 2] * 0.5f
							+ new_positions[(row * 2 + 1) * new_width + col * 2] * 0.5f
							+ new_positions[row * 2 * new_width + col * 2 - 1] * 0.5f
							+ new_positions[row * 2 * new_width + col * 2 + 1] * 0.5f
							+ new_positions[(row * 2 - 1) * new_width + col * 2 - 1] * 0.5f
							+ new_positions[(row * 2 + 1) * new_width + col * 2 + 1] * 0.5f) / 8.f;

					new_normals[row * 2 * new_width + col * 2] = (normals[row * width + col] * 5.f
							+ new_normals[(row * 2 - 1) * new_width + col * 2] * 0.5f
							+ new_normals[(row * 2 + 1) * new_width + col * 2] * 0.5f
							+ new_normals[row * 2 * new_width + col * 2 - 1] * 0.5f
							+ new_normals[row * 2 * new_width + col * 2 + 1] * 0.5f
							+ new_normals[(row * 2 - 1) * new_width + col * 2 - 1] * 0.5f
							+ new_normals[(row * 2 + 1) * new_width + col * 2 + 1] * 0.5f) / 8.f;

					new_texture_coordinates[row * 2 * new_width + col * 2] = (texture_coordinates[row * width + col] * 5.f
							+ new_texture_coordinates[(row * 2 - 1) * new_width + col * 2] * 0.5f
							+ new_texture_coordinates[(row * 2 + 1) * new_width + col * 2] * 0.5f
							+ new_texture_coordinates[row * 2 * new_width + col * 2 - 1] * 0.5f
							+ new_texture_coordinates[row * 2 * new_width + col * 2 + 1] * 0.5f
							+ new_texture_coordinates[(row * 2 - 1) * new_width + col * 2 - 1] * 0.5f
							+ new_texture_coordinates[(row * 2 + 1) * new_width + col * 2 + 1] * 0.5f) / 8.f;
				}
			}
		}

		positions = new_positions;
		normals = new_normals;
		texture_coordinates = new_texture_coordinates;
		width = width * 2 - 1;
		height = height * 2 - 1;
	}

	void RegularMesh::catmull_clark_subdivision()
	{
		if(width <= 2 || height <= 2)
		{
			std::cout << "RegularMesh: Catmull-clark subdivision does not work on meshes smaller than 3x3\n";
			throw std::runtime_error{"Regular mesh catmull-clark subdivision failed."};
		}
		auto new_width = width * 2 - 1;
		auto new_height = height * 2 - 1;
		auto new_positions = std::vector<glm::vec3>(new_width * (height * 2 - 1));
		auto new_normals = std::vector<glm::vec3>(new_width * (height * 2 - 1));
		auto new_texture_coordinates = std::vector<glm::vec2>(new_width * (height * 2 - 1));


		// TODO:change ugly unsigned arithmetic
		auto get_index = [] (size_t row, size_t col, size_t width, size_t height) {
			if(row == static_cast<size_t>(-1)) row = 1;
			if(col == static_cast<size_t>(-1)) col = 1;
			if(row == height) row = height - 2;
			if(col == width) col = width - 2;
			return row * width + col;
		};

		// Faces
		for(size_t row = 0; row < height; ++row)
		{
			for(size_t col = 0; col < width; ++col)
			{
				if(row + 1 < height && col + 1 < width)
				{
					new_positions[get_index(row * 2 + 1, col * 2 + 1, new_width, new_height)] = (positions[row * width + col]
							+ positions[row * width + col + 1]
							+ positions[(row + 1) * width + col]
							+ positions[(row + 1) * width + col + 1]) / 4.f;

					new_normals[get_index(row * 2 + 1, col * 2 + 1, new_width, new_height)] = (normals[row * width + col]
							+ normals[row * width + col + 1]
							+ normals[(row + 1) * width + col]
							+ normals[(row + 1) * width + col + 1]) / 4.f;

					new_texture_coordinates[get_index(row * 2 + 1, col * 2 + 1, new_width, new_height)] = (texture_coordinates[row * width + col]
							+ texture_coordinates[row * width + col + 1]
							+ texture_coordinates[(row + 1) * width + col]
							+ texture_coordinates[(row + 1) * width + col + 1]) / 4.f;
				}
			}
		}

		// Edges
		for(size_t row = 0; row < height; ++row)
		{
			for(size_t col = 0; col < width; ++col)
			{
				// Right edge
				new_positions[get_index(row * 2, col * 2 + 1, new_width, new_height)] = (positions[get_index(row, col, width, height)]
						+ positions[get_index(row, col + 1, width, height)]
						+ new_positions[get_index(row * 2 - 1, col * 2 + 1, new_width, new_height)]
						+ new_positions[get_index(row * 2 + 1, col * 2 + 1, new_width, new_height)]) / 4.f;

				new_normals[get_index(row * 2, col * 2 + 1, new_width, new_height)] = (normals[get_index(row, col, width, height)]
						+ normals[get_index(row, col + 1, width, height)]
						+ new_normals[get_index(row * 2 - 1, col * 2 + 1, new_width, new_height)]
						+ new_normals[get_index(row * 2 + 1, col * 2 + 1, new_width, new_height)]) / 4.f;

				new_texture_coordinates[get_index(row * 2, col * 2 + 1, new_width, new_height)] = (texture_coordinates[get_index(row, col, width, height)]
						+ texture_coordinates[get_index(row, col + 1, width, height)]
						+ new_texture_coordinates[get_index(row * 2 - 1, col * 2 + 1, new_width, new_height)]
						+ new_texture_coordinates[get_index(row * 2 + 1, col * 2 + 1, new_width, new_height)]) / 4.f;

				// Down edge
				new_positions[get_index(row * 2 + 1, col * 2, new_width, new_height)] = (positions[get_index(row, col, width, height)]
						+ positions[get_index(row + 1, col, width, height)]
						+ new_positions[get_index(row * 2 + 1, col * 2 - 1, new_width, new_height)]
						+ new_positions[get_index(row * 2 + 1, col * 2 + 1, new_width, new_height)]) / 4.f;

				new_normals[get_index(row * 2 + 1, col * 2, new_width, new_height)] = (normals[get_index(row, col, width, height)]
						+ normals[get_index(row + 1, col, width, height)]
						+ new_normals[get_index(row * 2 + 1, col * 2 - 1, new_width, new_height)]
						+ new_normals[get_index(row * 2 + 1, col * 2 + 1, new_width, new_height)]) / 4.f;

				new_texture_coordinates[get_index(row * 2 + 1, col * 2, new_width, new_height)] = (texture_coordinates[get_index(row, col, width, height)]
						+ texture_coordinates[get_index(row + 1, col, width, height)]
						+ new_texture_coordinates[get_index(row * 2 + 1, col * 2 - 1, new_width, new_height)]
						+ new_texture_coordinates[get_index(row * 2 + 1, col * 2 + 1, new_width, new_height)]) / 4.f;
			}
		}

		// Old vertices
		for(size_t row = 0; row < height; ++row)
		{
			for(size_t col = 0; col < width; ++col)
			{
				// Horizontal boundary vertex
				new_positions[get_index(row * 2, col * 2, new_width, new_height)] = (positions[get_index(row, col, width, height)] * 8.f
						+ new_positions[get_index(row * 2, col * 2 - 1, new_width, new_height)]
						+ new_positions[get_index(row * 2, col * 2 + 1, new_width, new_height)]
						+ new_positions[get_index(row * 2 - 1, col * 2, new_width, new_height)]
						+ new_positions[get_index(row * 2 + 1, col * 2, new_width, new_height)]
						+ new_positions[get_index(row * 2 - 1, col * 2 - 1, new_width, new_height)]
						+ new_positions[get_index(row * 2 - 1, col * 2 + 1, new_width, new_height)]
						+ new_positions[get_index(row * 2 + 1, col * 2 - 1, new_width, new_height)]
						+ new_positions[get_index(row * 2 + 1, col * 2 + 1, new_width, new_height)]) / 16.f;

				new_normals[get_index(row * 2, col * 2, new_width, new_height)] = (normals[get_index(row, col, width, height)] * 8.f
						+ new_normals[get_index(row * 2, col * 2 - 1, new_width, new_height)]
						+ new_normals[get_index(row * 2, col * 2 + 1, new_width, new_height)]
						+ new_normals[get_index(row * 2 - 1, col * 2, new_width, new_height)]
						+ new_normals[get_index(row * 2 + 1, col * 2, new_width, new_height)]
						+ new_normals[get_index(row * 2 - 1, col * 2 - 1, new_width, new_height)]
						+ new_normals[get_index(row * 2 - 1, col * 2 + 1, new_width, new_height)]
						+ new_normals[get_index(row * 2 + 1, col * 2 - 1, new_width, new_height)]
						+ new_normals[get_index(row * 2 + 1, col * 2 + 1, new_width, new_height)]) / 16.f;

				new_texture_coordinates[get_index(row * 2, col * 2, new_width, new_height)] = (texture_coordinates[get_index(row, col, width, height)] * 8.f
						+ new_texture_coordinates[get_index(row * 2, col * 2 - 1, new_width, new_height)]
						+ new_texture_coordinates[get_index(row * 2, col * 2 + 1, new_width, new_height)]
						+ new_texture_coordinates[get_index(row * 2 - 1, col * 2, new_width, new_height)]
						+ new_texture_coordinates[get_index(row * 2 + 1, col * 2, new_width, new_height)]
						+ new_texture_coordinates[get_index(row * 2 - 1, col * 2 - 1, new_width, new_height)]
						+ new_texture_coordinates[get_index(row * 2 - 1, col * 2 + 1, new_width, new_height)]
						+ new_texture_coordinates[get_index(row * 2 + 1, col * 2 - 1, new_width, new_height)]
						+ new_texture_coordinates[get_index(row * 2 + 1, col * 2 + 1, new_width, new_height)]) / 16.f;
			}
		}

		positions = new_positions;
		normals = new_normals;
		texture_coordinates = new_texture_coordinates;
		width = new_width;
		height = new_height;
	}

	void RegularMesh::catmull_clark_subdivision_sharp_bounds()
	{
		if(width == 1 || height == 1)
		{
			std::cout << "RegularMesh: Catmull-clark subdivision with sharp bounds does not work on meshes smaller than 2x2\n";
			throw std::runtime_error{"Regular mesh catmull-clark subdivision with sharp bounds failed."};
		}
		auto new_width = width * 2 - 1;
		auto new_positions = std::vector<glm::vec3>(new_width * (height * 2 - 1));
		auto new_normals = std::vector<glm::vec3>(new_width * (height * 2 - 1));
		auto new_texture_coordinates = std::vector<glm::vec2>(new_width * (height * 2 - 1));

		// Faces
		for(size_t row = 0; row < height; ++row)
		{
			for(size_t col = 0; col < width; ++col)
			{
				if(row + 1 < height && col + 1 < width)
				{
					new_positions[(row * 2 + 1) * new_width + col * 2 + 1] = (positions[row * width + col]
							+ positions[row * width + col + 1]
							+ positions[(row + 1) * width + col]
							+ positions[(row + 1) * width + col + 1]) / 4.f;

					new_normals[(row * 2 + 1) * new_width + col * 2 + 1] = (normals[row * width + col]
							+ normals[row * width + col + 1]
							+ normals[(row + 1) * width + col]
							+ normals[(row + 1) * width + col + 1]) / 4.f;

					new_texture_coordinates[(row * 2 + 1) * new_width + col * 2 + 1] = (texture_coordinates[row * width + col]
							+ texture_coordinates[row * width + col + 1]
							+ texture_coordinates[(row + 1) * width + col]
							+ texture_coordinates[(row + 1) * width + col + 1]) / 4.f;
				}
			}
		}

		// Edges
		for(size_t row = 0; row < height; ++row)
		{
			for(size_t col = 0; col < width; ++col)
			{
				// Right edge
				// Horizontal boundary
				if((row == 0 || row + 1 == height) && col + 1 < width)
				{
					new_positions[row * 2 * new_width + col * 2 + 1] = (positions[row * width + col]
							+ positions[row * width + col + 1]) / 2.f;

					new_normals[row * 2 * new_width + col * 2 + 1] = (normals[row * width + col]
							+ normals[row * width + col + 1]) / 2.f;

					new_texture_coordinates[row * 2 * new_width + col * 2 + 1] = (texture_coordinates[row * width + col]
							+ texture_coordinates[row * width + col + 1]) / 2.f;
				}
				// Inside
				else if(col + 1 < width)
				{
					new_positions[row * 2 * new_width + col * 2 + 1] = (positions[row * width + col]
							+ positions[row * width + col + 1]
							+ new_positions[(row * 2 - 1) * new_width + col * 2 + 1]
							+ new_positions[(row * 2 + 1) * new_width + col * 2 + 1]) / 4.f;

					new_normals[row * 2 * new_width + col * 2 + 1] = (normals[row * width + col]
							+ normals[row * width + col + 1]
							+ new_normals[(row * 2 - 1) * new_width + col * 2 + 1]
							+ new_normals[(row * 2 + 1) * new_width + col * 2 + 1]) / 4.f;

					new_texture_coordinates[row * 2 * new_width + col * 2 + 1] = (texture_coordinates[row * width + col]
							+ texture_coordinates[row * width + col + 1]
							+ new_texture_coordinates[(row * 2 - 1) * new_width + col * 2 + 1]
							+ new_texture_coordinates[(row * 2 + 1) * new_width + col * 2 + 1]) / 4.f;
				}

				// Down edge
				// Vertical boundary
				if((col == 0 || col + 1 == width) && row + 1 < height)
				{
					new_positions[(row * 2 + 1) * new_width + col * 2] = (positions[row * width + col]
							+ positions[(row + 1) * width + col]) / 2.f;

					new_normals[(row * 2 + 1) * new_width + col * 2] = (normals[row * width + col]
							+ normals[(row + 1) * width + col]) / 2.f;

					new_texture_coordinates[(row * 2 + 1) * new_width + col * 2] = (texture_coordinates[row * width + col]
							+ texture_coordinates[(row + 1) * width + col]) / 2.f;
				}
				// Inside
				else if(row + 1 < height)
				{
					new_positions[(row * 2 + 1) * new_width + col * 2] = (positions[row * width + col]
							+ positions[(row + 1) * width + col]
							+ new_positions[(row * 2 + 1) * new_width + col * 2 - 1]
							+ new_positions[(row * 2 + 1) * new_width + col * 2 + 1]) / 4.f;

					new_normals[(row * 2 + 1) * new_width + col * 2] = (normals[row * width + col]
							+ normals[(row + 1) * width + col]
							+ new_normals[(row * 2 + 1) * new_width + col * 2 - 1]
							+ new_normals[(row * 2 + 1) * new_width + col * 2 + 1]) / 4.f;

					new_texture_coordinates[(row * 2 + 1) * new_width + col * 2] = (texture_coordinates[row * width + col]
							+ texture_coordinates[(row + 1) * width + col]
							+ new_texture_coordinates[(row * 2 + 1) * new_width + col * 2 - 1]
							+ new_texture_coordinates[(row * 2 + 1) * new_width + col * 2 + 1]) / 4.f;
				}
			}
		}

		// Old vertices
		for(size_t row = 0; row < height; ++row)
		{
			for(size_t col = 0; col < width; ++col)
			{
				// Horizontal boundary vertex
				if((row == 0 || row + 1 == height) && col > 0 && col + 1 < width)
				{
					new_positions[row * 2 * new_width + col * 2] = (positions[row * width + col] * 6.f
							+ new_positions[row * 2 * new_width + col * 2 - 1]
							+ new_positions[row * 2 * new_width + col * 2 + 1]) / 8.f;

					new_normals[row * 2 * new_width + col * 2] = (normals[row * width + col] * 6.f
							+ new_normals[row * 2 * new_width + col * 2 - 1]
							+ new_normals[row * 2 * new_width + col * 2 + 1]) / 8.f;

					new_texture_coordinates[row * 2 * new_width + col * 2] = (texture_coordinates[row * width + col] * 6.f
							+ new_texture_coordinates[row * 2 * new_width + col * 2 - 1]
							+ new_texture_coordinates[row * 2 * new_width + col * 2 + 1]) / 8.f;
				}
				// Vertical boundary vertex
				else if((col == 0 || col + 1 == width) && row > 0 && row + 1 < height)
				{
					new_positions[row * 2 * new_width + col * 2] = (positions[row * width + col] * 6.f
							+ new_positions[(row * 2 - 1) * new_width + col * 2]
							+ new_positions[(row * 2 + 1) * new_width + col * 2]) / 8.f;

					new_normals[row * 2 * new_width + col * 2] = (normals[row * width + col] * 6.f
							+ new_normals[(row * 2 - 1) * new_width + col * 2]
							+ new_normals[(row * 2 + 1) * new_width + col * 2]) / 8.f;

					new_texture_coordinates[row * 2 * new_width + col * 2] = (texture_coordinates[row * width + col] * 6.f
							+ new_texture_coordinates[(row * 2 - 1) * new_width + col * 2]
							+ new_texture_coordinates[(row * 2 + 1) * new_width + col * 2]) / 8.f;
				}
				// Corner vertex
				else if(row == 0 || row + 1 == height || col == 0 || col + 1 == width)
				{
					new_positions[row * 2 * new_width + col * 2] = positions[row * width + col];

					new_normals[row * 2 * new_width + col * 2] = normals[row * width + col];

					new_texture_coordinates[row * 2 * new_width + col * 2] = texture_coordinates[row * width + col];
				}
				// Regular inside vertex
				else
				{
					new_positions[row * 2 * new_width + col * 2] = (positions[row * width + col] * 8.f
							+ new_positions[row * 2 * new_width + col * 2 - 1]
							+ new_positions[row * 2 * new_width + col * 2 + 1]
							+ new_positions[(row * 2 - 1) * new_width + col * 2]
							+ new_positions[(row * 2 + 1)* new_width + col * 2]
							+ new_positions[(row * 2 - 1) * new_width + col * 2 - 1]
							+ new_positions[(row * 2 - 1) * new_width + col * 2 + 1]
							+ new_positions[(row * 2 + 1) * new_width + col * 2 - 1]
							+ new_positions[(row * 2 + 1) * new_width + col * 2 + 1]) / 16.f;

					new_normals[row * 2 * new_width + col * 2] = (normals[row * width + col] * 8.f
							+ new_normals[row * 2 * new_width + col * 2 - 1]
							+ new_normals[row * 2 * new_width + col * 2 + 1]
							+ new_normals[(row * 2 - 1) * new_width + col * 2]
							+ new_normals[(row * 2 + 1)* new_width + col * 2]
							+ new_normals[(row * 2 - 1) * new_width + col * 2 - 1]
							+ new_normals[(row * 2 - 1) * new_width + col * 2 + 1]
							+ new_normals[(row * 2 + 1) * new_width + col * 2 - 1]
							+ new_normals[(row * 2 + 1) * new_width + col * 2 + 1]) / 16.f;

					new_texture_coordinates[row * 2 * new_width + col * 2] = (texture_coordinates[row * width + col] * 8.f
							+ new_texture_coordinates[row * 2 * new_width + col * 2 - 1]
							+ new_texture_coordinates[row * 2 * new_width + col * 2 + 1]
							+ new_texture_coordinates[(row * 2 - 1) * new_width + col * 2]
							+ new_texture_coordinates[(row * 2 + 1)* new_width + col * 2]
							+ new_texture_coordinates[(row * 2 - 1) * new_width + col * 2 - 1]
							+ new_texture_coordinates[(row * 2 - 1) * new_width + col * 2 + 1]
							+ new_texture_coordinates[(row * 2 + 1) * new_width + col * 2 - 1]
							+ new_texture_coordinates[(row * 2 + 1) * new_width + col * 2 + 1]) / 16.f;
				}
			}
		}

		positions = new_positions;
		normals = new_normals;
		texture_coordinates = new_texture_coordinates;
		width = width * 2 - 1;
		height = height * 2 - 1;
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
