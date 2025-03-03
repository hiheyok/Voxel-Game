#pragma once
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <string>
#include "../../Level/Typenames.h"

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
	glm::ivec3 normal_ = glm::ivec3(0, 0, 0);
	std::string reference_texture_ = ""; // variables starts with #
	int cull_face_ = -1;
	int tint_index_ = -1;
	size_t texture_id_ = NULL;
	size_t texture_count_ = NULL; //For amination purposes.
	unsigned int rotation_ = 0;
	bool has_transparency_ = false;
	bool is_see_through_ = false;
	glm::ivec4 uv_ = glm::ivec4(0, 0, 16, 16);

	glm::ivec2 uv_coord_nn{ 0, 0 };
	glm::ivec2 uv_coord_np{ 0, 0 };
	glm::ivec2 uv_coord_pp{ 0, 0 };
	glm::ivec2 uv_coord_pn{ 0, 0 };
};

struct BlockDisplay { //Positions {thirdperson_righthand, thirdperson_lefthand, firstperson_righthand, firstperson_lefthand, gui, head, ground, or fixed}
	//fixed = item frame
	DisplayPosition position; // TODO: Rename this
	glm::vec3 rotation_ = glm::vec3(0.f, 0.f, 0.f);
	glm::vec3 translation_ = glm::vec3(0.f, 0.f, 0.f);
	glm::vec3 scale_ = glm::vec3(0.f, 0.f, 0.f);
	bool initialized_ = false;
};

struct CuboidRotationInfo {
	glm::vec3 origin_ = glm::vec3(0.f, 0.f, 0.f);
	int axis_ = 0;
	int angle_ = 0;
	bool rescale_ = false;
	bool initialized_ = false;
};

class Cuboid {
public:
	glm::ivec3 from_ = glm::ivec3(0,0,0);
	glm::ivec3 to_ = glm::ivec3(16, 16, 16); //Relative inner block position from -16 to 32
	BlockFace faces_[6]{};
	CuboidRotationInfo rotation_;
	std::string comments_ = "";
	bool shade_ = true;

	void EditFace(int location, BlockFace f) {
		faces_[location - 1] = f;
	}
};

namespace ModelV2 {
	struct BlockModelV2 {
		std::vector<Cuboid> elements_{};
		bool ambient_occlusion_ = true; //default value
		bool is_initialized_ = false;
		FastHashMap<std::string, std::string > texture_variable_;
		BlockDisplay display_[8]{};

		void AddDisplay(BlockDisplay display, DisplayPosition position) {
			display_[position] = display;
		}

		bool CheckDisplay(DisplayPosition position) {
			return display_[position].initialized_;
		}

		void AddElement(Cuboid element) {
			elements_.push_back(element);
		}

		void GetVertices(std::vector<float>& vertices, std::vector<unsigned int>& indices){
			for (Cuboid& element : elements_) {
				glm::vec3 from = element.from_;
				glm::vec3 to = element.to_;
				from = from / 16.f;
				to = to / 16.f;
				for (int i = 0; i < 6;  i++) {
					BlockFace face = element.faces_[i];
					if (face.reference_texture_.length() == 0) continue;

					glm::vec2 uv0{ face.uv_.x,  face.uv_.y };
					glm::vec2 uv1{ face.uv_.z,  face.uv_.w };

					uv0 = uv0 / 16.f;
					uv1 = uv1 / 16.f;

					uint8_t axis_ = i >> 1;

					uint32_t currIndex = static_cast<uint32_t>(vertices.size()) / 7;

					switch (axis_) {
					case 0:
						(i & 1) == 1 ?
							vertices.insert(vertices.end(),
								{
									from.x,	from.y, from.z,		uv1.x, uv0.y,	(float)face.texture_id_,	8.f,
									from.x, to.y,	from.z,		uv1.x, uv1.y,	(float)face.texture_id_,	8.f,
									from.x, to.y,	to.z,		uv0.x, uv1.y,	(float)face.texture_id_,	8.f,
									from.x, from.y,	to.z,		uv0.x, uv0.y,	(float)face.texture_id_,	8.f,
								}
								) :
							vertices.insert(vertices.end(),
								{
									to.x, from.y,   from.z,		uv1.x, uv0.y,	(float)face.texture_id_,	8.f,
									to.x, to.y,		from.z,		uv1.x, uv1.y,	(float)face.texture_id_,	8.f,
									to.x, to.y,		to.z,		uv0.x, uv1.y,	(float)face.texture_id_,	8.f,
									to.x, from.y,	to.z,		uv0.x, uv0.y,	(float)face.texture_id_,	8.f,
								}
						);
						break;
					case 1:
						(i & 1) == 1 ?
							vertices.insert(vertices.end(),
								{
									from.x, from.y, from.z,		uv1.x, uv0.y,	(float)face.texture_id_,	15.f,
									to.x,	from.y, from.z,		uv0.x, uv0.y,	(float)face.texture_id_,	15.f,
									to.x,	from.y, to.z,		uv0.x, uv1.y,	(float)face.texture_id_,	15.f,
									from.x, from.y, to.z,		uv1.x, uv1.y,	(float)face.texture_id_,	15.f,
								}
								) :
							vertices.insert(vertices.end(),
								{
									from.x, to.y,	from.z,		uv1.x, uv0.y,	(float)face.texture_id_,	15.f,
									to.x,	to.y,	from.z,		uv0.x, uv0.y,	(float)face.texture_id_,	15.f,
									to.x,	to.y,	to.z,		uv0.x, uv1.y,	(float)face.texture_id_,	15.f,
									from.x, to.y,	to.z,		uv1.x, uv1.y,	(float)face.texture_id_,	15.f,
								}
						);
						break;
					case 2:
						(i & 1) == 1 ?
							vertices.insert(vertices.end(),
								{
									from.x, from.y, from.z,		uv1.x, uv0.y,	(float)face.texture_id_,	12.f,
									to.x,	from.y, from.z,		uv0.x, uv0.y,	(float)face.texture_id_,	12.f,
									to.x,	to.y,	from.z,		uv0.x, uv1.y,	(float)face.texture_id_,	12.f,
									from.x, to.y,	from.z,		uv1.x, uv1.y,	(float)face.texture_id_,	12.f,
								}
								) :
							vertices.insert(vertices.end(),
								{
									from.x, from.y, to.z,		uv1.x, uv0.y,	(float)face.texture_id_,	12.f,
									to.x,	from.y, to.z,		uv0.x, uv0.y,	(float)face.texture_id_,	12.f,
									to.x,	to.y,	to.z,		uv0.x, uv1.y,	(float)face.texture_id_,	12.f,
									from.x, to.y,	to.z,		uv1.x, uv1.y,	(float)face.texture_id_,	12.f,
								}
						);
						break;

					}

					if ((i & 1) == 0) {
						indices.insert(indices.end(),
							{
								0 + currIndex, 1 + currIndex, 2 + currIndex,
								2 + currIndex, 3 + currIndex, 0 + currIndex
							}
						);
					}
				}
			}
		}

		void FlattenVariables();

		void BakeTextureRotation();
	};
}

