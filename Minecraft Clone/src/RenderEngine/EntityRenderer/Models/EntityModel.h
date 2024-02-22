#pragma once

#include <glm/vec3.hpp>
#include <iostream>
#include <vector>

namespace Model {
	struct ModelData {
		std::vector<float> Vertices;
		std::vector<unsigned int> Indices;

		int VerticesCount = 0;
	};

	struct RectangularPrism {

		RectangularPrism(float x, float y, float z) {
			size.x = x;
			size.y = y;
			size.z = z;
		}

		RectangularPrism(glm::vec3 _size, glm::vec3 _offset) {
			size = _size;
			offset = _offset;
		}

		ModelData GetVertices();

		glm::vec3 offset = glm::vec3(0.f, 0.f, 0.f);

		glm::vec3 size = glm::vec3(0.f, 0.f, 0.f);
	};
}

class EntityModel {
public:
	
	void AddRectangle(glm::vec3 size, glm::vec3 offset) {
		shapes.emplace_back(size, offset);
	}

	Model::ModelData GetVertices() {

		Model::ModelData model;

		for (Model::RectangularPrism& shape : shapes) {
			Model::ModelData SubModel = shape.GetVertices();

			int CurrentIndex = model.VerticesCount;
			
			for (int i = 0; i < SubModel.Vertices.size(); i++) {
				model.Vertices.push_back(SubModel.Vertices[i] + shape.offset[i % 3]);
			}

			for (int i = 0; i < SubModel.Indices.size(); i++) {
				model.Indices.push_back(SubModel.Indices[i] + CurrentIndex);
			}

			model.VerticesCount += SubModel.VerticesCount;

		}

		return model;
	}

private:
	std::vector<Model::RectangularPrism> shapes;
};