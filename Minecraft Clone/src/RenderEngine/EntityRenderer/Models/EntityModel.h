#pragma once

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <iostream>
#include <vector>

namespace Model {
	typedef struct  {
		std::vector<float> Vertices;
		std::vector<unsigned int> Indices;

		int VerticesCount = 0;
	} ModelData;

	typedef struct {
		glm::vec2 p0 = glm::vec2(0.f, 0.f);
		glm::vec2 p1 = glm::vec2(0.f, 0.f);
	} UV;

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

		UV UV_MAP[6]{};
	};
}

class EntityModel {
public:

	Model::RectangularPrism* AddRectangle(glm::vec3 size, glm::vec3 offset) {
		shapes.emplace_back(size, offset);
		return &shapes.back();
	}

	Model::ModelData GetVertices() {

		Model::ModelData model;

		for (Model::RectangularPrism& shape : shapes) {
			Model::ModelData SubModel = shape.GetVertices();

			int CurrentIndex = model.VerticesCount;

			for (int i = 0; i < SubModel.Vertices.size(); i++) {
				if ((i % 5) < 3) {
					model.Vertices.push_back(SubModel.Vertices[i] + shape.offset[i % 5]);
				}
				else {
					model.Vertices.push_back(SubModel.Vertices[i]);
				}
				
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