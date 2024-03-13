#pragma once
#include <string>
#include <glm/vec2.hpp>
#include <vector>



struct GUIElement {
	struct GUIVertices {
		std::vector<float> Vertices = {};
		std::vector<int> Indices = {};
	};

	GUIElement() {

	}

	GUIElement(std::string text, glm::vec2 size, glm::vec2 location) : Text(text), Size(size), Location(location) {

	}

	GUIVertices GetVertices() {
		GUIVertices data;
		UV_P0.y = 1.f - UV_P0.y;
		UV_P1.y = 1.f - UV_P1.y;

		data.Vertices.insert(data.Vertices.end(),

			{
				Location.x - (Size.x / 2.f), Location.y - (Size.y / 2.f), UV_P0.x, UV_P1.y,
				Location.x + (Size.x / 2.f), Location.y - (Size.y / 2.f), UV_P1.x, UV_P1.y,
				Location.x + (Size.x / 2.f), Location.y + (Size.y / 2.f), UV_P1.x, UV_P0.y,
				Location.x - (Size.x / 2.f), Location.y + (Size.y / 2.f), UV_P0.x, UV_P0.y,
			});

		data.Indices.insert(data.Indices.end(),
			{
				0, 1, 2,
				2, 3, 0
			}
			);


		UV_P0.y = 1.f - UV_P0.y;
		UV_P1.y = 1.f - UV_P1.y;

		return data;
	}

	std::string Text = "";
	glm::vec2 Size = glm::vec2(0.f, 0.f);;
	glm::vec2 Location = glm::vec2(0.f,0.f);
	glm::vec2 UV_P0 = glm::vec2(0.f, 0.f);
	glm::vec2 UV_P1 = glm::vec2(0.f, 0.f);

	int Texture = 0;
	bool hasTexture = false;
	bool isButton = 0;

	int BufferIndex = 0;
};