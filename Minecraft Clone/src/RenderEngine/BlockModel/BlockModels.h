#pragma once
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <string>
#include <unordered_map>

#define EAST 0x01
#define WEST 0x02
#define UP 0x03
#define DOWN 0x04
#define SOUTH 0x05
#define NORTH 0x06

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
	int CullFace = -1;
	int TintIndex = -1;
	unsigned int TextureID = NULL;
	unsigned int TextureCount = NULL; //For amination purposes.
	unsigned int rotation = 0;
	bool hasTransparency = false;
	bool isSeeThrough = false;
	glm::ivec4 UV = glm::ivec4(0, 0, 16, 16);

	glm::ivec2 UVCoordNN{ 0, 0 };
	glm::ivec2 UVCoordNP{ 0, 0 };
	glm::ivec2 UVCoordPP{ 0, 0 };
	glm::ivec2 UVCoordPN{ 0, 0 };
};

struct BlockDisplay { //Positions {thirdperson_righthand, thirdperson_lefthand, firstperson_righthand, firstperson_lefthand, gui, head, ground, or fixed}
	//fixed = item frame
	DisplayPosition position;
	glm::vec3 rotation = glm::vec3(0.f, 0.f, 0.f);
	glm::vec3 translation = glm::vec3(0.f, 0.f, 0.f);
	glm::vec3 scale = glm::vec3(0.f, 0.f, 0.f);
	bool Initialized = false;
};

struct CuboidRotationInfo {
	glm::vec3 origin = glm::vec3(0.f, 0.f, 0.f);
	int axis = 0;
	int angle = 0;
	bool rescale = false;
	bool Initialized = false;
};

class Cuboid {
public:
	glm::ivec3 From = glm::ivec3(0,0,0);
	glm::ivec3 To = glm::ivec3(16, 16, 16); //Relative inner block position from -16 to 32
	BlockFace Faces[6]{};
	CuboidRotationInfo rotation;
	std::string comments = "";
	bool shade = true;

	void EditFace(int location, BlockFace f) {
		Faces[location - 1] = f;
	}
};

namespace ModelV2 {
	struct BlockModelV2 {
		std::vector<Cuboid> Elements{};
		bool AmbientOcclusion = true; //default value
		bool isInitialized = false;
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

		void getVertices(std::vector<float>& Vertices, std::vector<unsigned int>& Indices){
			for (Cuboid& element : Elements) {
				glm::vec3 from = element.From;
				glm::vec3 to = element.To;
				from = from / 16.f;
				to = to / 16.f;
				for (int i = 0; i < 6;  i++) {
					BlockFace face = element.Faces[i];
					if (face.ReferenceTexture.length() == 0) continue;

					glm::vec2 uv0{ face.UV.x,  face.UV.y };
					glm::vec2 uv1{ face.UV.z,  face.UV.w };

					uv0 = uv0 / 16.f;
					uv1 = uv1 / 16.f;

					uint8_t axis = i >> 1;

					unsigned int currIndex = Vertices.size() / 7;

					switch (axis) {
					case 0:
						(i & 1) == 1 ?
							Vertices.insert(Vertices.end(),
								{
									from.x,	from.y, from.z,		uv1.x, uv0.y,	(float)face.TextureID,	8.f,
									from.x, to.y,	from.z,		uv1.x, uv1.y,	(float)face.TextureID,	8.f,
									from.x, to.y,	to.z,		uv0.x, uv1.y,	(float)face.TextureID,	8.f,
									from.x, from.y,	to.z,		uv0.x, uv0.y,	(float)face.TextureID,	8.f,
								}
								) :
							Vertices.insert(Vertices.end(),
								{
									to.x, from.y,   from.z,		uv1.x, uv0.y,	(float)face.TextureID,	8.f,
									to.x, to.y,		from.z,		uv1.x, uv1.y,	(float)face.TextureID,	8.f,
									to.x, to.y,		to.z,		uv0.x, uv1.y,	(float)face.TextureID,	8.f,
									to.x, from.y,	to.z,		uv0.x, uv0.y,	(float)face.TextureID,	8.f,
								}
						);
					case 1:
						(i & 1) == 1 ?
							Vertices.insert(Vertices.end(),
								{
									from.x, from.y, from.z,		uv1.x, uv0.y,	(float)face.TextureID,	15.f,
									to.x,	from.y, from.z,		uv0.x, uv0.y,	(float)face.TextureID,	15.f,
									to.x,	from.y, to.z,		uv0.x, uv1.y,	(float)face.TextureID,	15.f,
									from.x, from.y, to.z,		uv1.x, uv1.y,	(float)face.TextureID,	15.f,
								}
								) :
							Vertices.insert(Vertices.end(),
								{
									from.x, to.y,	from.z,		uv1.x, uv0.y,	(float)face.TextureID,	15.f,
									to.x,	to.y,	from.z,		uv0.x, uv0.y,	(float)face.TextureID,	15.f,
									to.x,	to.y,	to.z,		uv0.x, uv1.y,	(float)face.TextureID,	15.f,
									from.x, to.y,	to.z,		uv1.x, uv1.y,	(float)face.TextureID,	15.f,
								}
						);
					case 2:
						(i & 1) == 1 ?
							Vertices.insert(Vertices.end(),
								{
									from.x, from.y, from.z,		uv1.x, uv0.y,	(float)face.TextureID,	12.f,
									to.x,	from.y, from.z,		uv0.x, uv0.y,	(float)face.TextureID,	12.f,
									to.x,	to.y,	from.z,		uv0.x, uv1.y,	(float)face.TextureID,	12.f,
									from.x, to.y,	from.z,		uv1.x, uv1.y,	(float)face.TextureID,	12.f,
								}
								) :
							Vertices.insert(Vertices.end(),
								{
									from.x, from.y, to.z,		uv1.x, uv0.y,	(float)face.TextureID,	12.f,
									to.x,	from.y, to.z,		uv0.x, uv0.y,	(float)face.TextureID,	12.f,
									to.x,	to.y,	to.z,		uv0.x, uv1.y,	(float)face.TextureID,	12.f,
									from.x, to.y,	to.z,		uv1.x, uv1.y,	(float)face.TextureID,	12.f,
								}
						);

					}

					if ((i & 1) == 0) {
						Indices.insert(Indices.end(),
							{
								0 + currIndex, 1 + currIndex, 2 + currIndex,
								2 + currIndex, 3 + currIndex, 0 + currIndex
							}
						);
					}
					

				}
			}
		}

		void flattenVariables();

		void bakeTextureRotation();
	};
}

