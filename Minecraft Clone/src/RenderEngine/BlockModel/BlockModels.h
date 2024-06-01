#pragma once
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <string>
#include <unordered_map>

#define DOWN 0x01
#define UP 0x02
#define NORTH 0x03
#define SOUTH 0x04
#define WEST 0x05
#define EAST 0x06

enum DisplayPosition {
	thirdperson_righthand = 0,
	thirdperson_lefthand = 1,
	firstperson_righthand = 2,
	firstperson_lefthand = 3,
	gui = 4,
	head = 5,
	ground = 6,
	fixed = 7
};

struct BlockFace {
	glm::ivec3 Normal = glm::ivec3(0, 0, 0);
	std::string ReferenceTexture = ""; // variables starts with #
	glm::ivec4 UV = glm::ivec4(0, 0, 16, 16);
};

struct BlockDisplay { //Positions {thirdperson_righthand, thirdperson_lefthand, firstperson_righthand, firstperson_lefthand, gui, head, ground, or fixed}
	//fixed = item frame
	DisplayPosition position;
	glm::vec3 rotation = glm::vec3(0.f, 0.f, 0.f);
	glm::vec3 translation = glm::vec3(0.f, 0.f, 0.f);
	glm::vec3 scale = glm::vec3(0.f, 0.f, 0.f);
	bool Initialized = false;

};

class Cuboid {
public:
	glm::ivec3 From = glm::ivec3(0,0,0);
	glm::ivec3 To = glm::ivec3(16, 16, 16); //Relative inner block position from -16 to 32
	BlockFace Faces[6]{};

	void EditFace(int location, BlockFace f) {
		Faces[location - 1] = f;
	}
};

namespace Model {
	struct BlockModel {
		bool ambientocclusion = true; //default value
		std::vector<Cuboid> Elements = {};
		std::unordered_map <std::string, std::string > TextureVariable;
		BlockDisplay Display[8]{};

		void AddDisplay(BlockDisplay display, DisplayPosition position) {
			Display[position] = display;
		}

		bool CheckDisplay(DisplayPosition position) {
			return Display[position].Initialized;
		}

		void AddElement(Cuboid element) {
			Elements.push_back(element);
		}
	};
}

