#pragma once
#include <string>
#include <glm/vec2.hpp>
#include <vector>



struct GUIElement {
	struct GUIVertices {
		std::vector<float> vertices_ = {};
		std::vector<int> indices_ = {};
	};

	GUIElement() {

	}

	GUIElement(std::string text, glm::vec2 size, glm::vec2 location) : text_(text), size_(size), location_(location) {

	}

	GUIVertices GetVertices() {
		GUIVertices data;
		uv_p0_.y = 1.f - uv_p0_.y;
		uv_p1_.y = 1.f - uv_p1_.y;

		data.vertices_.insert(data.vertices_.end(),

			{
				location_.x - (size_.x / 2.f), location_.y - (size_.y / 2.f), uv_p0_.x, uv_p1_.y,
				location_.x + (size_.x / 2.f), location_.y - (size_.y / 2.f), uv_p1_.x, uv_p1_.y,
				location_.x + (size_.x / 2.f), location_.y + (size_.y / 2.f), uv_p1_.x, uv_p0_.y,
				location_.x - (size_.x / 2.f), location_.y + (size_.y / 2.f), uv_p0_.x, uv_p0_.y,
			});

		data.indices_.insert(data.indices_.end(),
			{
				0, 1, 2,
				2, 3, 0
			}
			);


		uv_p0_.y = 1.f - uv_p0_.y;
		uv_p1_.y = 1.f - uv_p1_.y;

		return data;
	}

	std::string text_ = "";
	glm::vec2 size_ = glm::vec2(0.f, 0.f);;
	glm::vec2 location_ = glm::vec2(0.f,0.f);
	glm::vec2 uv_p0_ = glm::vec2(0.f, 0.f);
	glm::vec2 uv_p1_ = glm::vec2(0.f, 0.f);

	int texture_ = 0;
	bool has_texture_ = false;
	bool is_button_ = 0;

	int buffer_index_ = 0;
};