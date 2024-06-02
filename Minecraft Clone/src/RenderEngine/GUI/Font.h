#pragma once
#include <string>
#include <glm/vec4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <vector>

const float CharWidth[256]{
		1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f,
		1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f,
		0.625f, 0.125f, 0.250f, 0.625f, 0.625f, 0.625f, 0.625f, 0.125f, 0.375f, 0.375f, 0.375f, 0.625f, 0.125f, 0.625f, 0.125f, 0.625f,
		0.625f, 0.625f, 0.625f, 0.625f, 0.625f, 0.625f, 0.625f, 0.625f, 0.625f, 0.625f, 0.125f, 0.125f, 0.500f, 0.625f, 0.500f, 0.625f,
		0.750f, 0.625f, 0.625f, 0.625f, 0.625f, 0.625f, 0.625f, 0.625f, 0.625f, 0.375f, 0.625f, 0.625f, 0.625f, 0.625f, 0.625f, 0.625f,
		0.625f, 0.625f, 0.625f, 0.625f, 0.625f, 0.625f, 0.625f, 0.625f, 0.625f, 0.625f, 0.625f, 0.375f, 0.625f, 0.375f, 0.625f, 0.625f,
		0.250f, 0.625f, 0.625f, 0.625f, 0.625f, 0.625f, 0.500f, 0.625f, 0.625f, 0.125f, 0.625f, 0.500f, 0.250f, 0.625f, 0.625f, 0.625f,
		0.625f, 0.625f, 0.625f, 0.625f, 0.375f, 0.625f, 0.625f, 0.625f, 0.625f, 0.625f, 0.625f, 0.375f, 0.125f, 0.375f, 0.625f, 1.000f,
		1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f,
		1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 0.625f, 1.000f, 1.000f, 0.625f,
		1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 0.500f, 0.500f, 1.000f, 1.000f, 0.625f, 1.000f, 1.000f, 1.000f, 0.750f, 0.750f,

		//Rows beyond this is just garbage data for now
		1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f,
		1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f,
		1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f,
		1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f,
		1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f, 1.000f,
};

class RenderableFont {
private:

	std::string RenderText = "";
	float fontSize = 0.0625f;
	float spacing = 0.125f;

	glm::vec4 GetCharUV(char c);
	glm::vec3 color = glm::vec3(1.f, 1.f, 1.f);
	glm::vec2 Position = glm::vec2(-1.f,-1.f);

	std::vector<float> GetCharactorVertex(char c, float xOffset, float yOffset, float xOrigin, float yOrigin);
	float GetCharWidth(char c);
public:
	void SetText(std::string str) {
		RenderText = str;
	}

	void FontSize(float size) {
		fontSize = size;
	}

	void SetColor(glm::vec3 clr) {
		color = clr;
	}

	void SetPosition(glm::vec2 pos) {
		Position = pos;
	}

	std::vector<float> getVertices(); //-1 - 1 coords
};