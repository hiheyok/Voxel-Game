#pragma once
#include "Texture/BlockTexture.h"
#include "../../Typenames.h"
#include <vector>

struct BlockType {

	BlockType(bool transparent, bool solid, bool fluid) {
		transparency = transparent;
		isSolid = solid;
		isFluid = fluid;
	}

	BlockType() {

	}

	bool isFluid = NULL;
	bool transparency = NULL;
	bool isSolid = NULL;
};

struct BlockModel {
	std::vector<float> Vertices;
	std::vector<unsigned int> Indices;
};

struct Block {

	virtual ~Block() {
		Clean();
	}

	void Clean() {
		if (Properties != nullptr) {
			delete Properties;
		}
		if (Texture != nullptr) {
			delete Texture;
		}
	}

	virtual void tick(int x, int y, int z) {

	}

	BlockModel GetRenderModel() {
		BlockModel model;
		model.Vertices.insert(model.Vertices.begin(),
			{ //vertex		UV		Tex									Light
				//Front
				0.f, 0.f, 0.f,	1.f, 0.f,	(float)Texture->GetFace(FRONT),	12.f,
				1.f, 0.f, 0.f,	0.f, 0.f,	(float)Texture->GetFace(FRONT),	12.f,
				1.f, 1.f, 0.f,	0.f, 1.f,	(float)Texture->GetFace(FRONT),	12.f,
				0.f, 1.f, 0.f,	1.f, 1.f,	(float)Texture->GetFace(FRONT),	12.f,

				//Back
				0.f, 0.f, 1.f,	1.f, 0.f,	(float)Texture->GetFace(BACK),	12.f,
				1.f, 0.f, 1.f,	0.f, 0.f,	(float)Texture->GetFace(BACK),	12.f,
				1.f, 1.f, 1.f,	0.f, 1.f,	(float)Texture->GetFace(BACK),	12.f,
				0.f, 1.f, 1.f,	1.f, 1.f,	(float)Texture->GetFace(BACK),	12.f,

				//Right
				1.f, 0.f, 0.f,	1.f, 0.f,	(float)Texture->GetFace(RIGHT),	8.f,
				1.f, 1.f, 0.f,	1.f, 1.f,	(float)Texture->GetFace(RIGHT),	8.f,
				1.f, 1.f, 1.f,	0.f, 1.f,	(float)Texture->GetFace(RIGHT),	8.f,
				1.f, 0.f, 1.f,	0.f, 0.f,	(float)Texture->GetFace(RIGHT),	8.f,

				//LEFT
				0.f, 0.f, 0.f,	1.f, 0.f,	(float)Texture->GetFace(LEFT),	8.f,
				0.f, 1.f, 0.f,	1.f, 1.f,	(float)Texture->GetFace(LEFT),	8.f,
				0.f, 1.f, 1.f,	0.f, 1.f,	(float)Texture->GetFace(LEFT),	8.f,
				0.f, 0.f, 1.f,	0.f, 0.f,	(float)Texture->GetFace(LEFT),	8.f,

				//TOP
				0.f, 1.f, 0.f,	1.f, 0.f,	(float)Texture->GetFace(TOP),	15.f,
				1.f, 1.f, 0.f,	0.f, 0.f,	(float)Texture->GetFace(TOP),	15.f,
				1.f, 1.f, 1.f,	0.f, 1.f,	(float)Texture->GetFace(TOP),	15.f,
				0.f, 1.f, 1.f,	1.f, 1.f,	(float)Texture->GetFace(TOP),	15.f,

				//BOTTOM
				0.f, 0.f, 0.f,	1.f, 0.f,	(float)Texture->GetFace(BOTTOM),	15.f,
				1.f, 0.f, 0.f,	0.f, 0.f,	(float)Texture->GetFace(BOTTOM),	15.f,
				1.f, 0.f, 1.f,	0.f, 1.f,	(float)Texture->GetFace(BOTTOM),	15.f,
				0.f, 0.f, 1.f,	1.f, 1.f,	(float)Texture->GetFace(BOTTOM),	15.f,
			});

		model.Indices.insert(model.Indices.begin(), {
			//Front
			//0,1,2,
			//2,3,0,

			//Back
			4,6,5,
			6,4,7,

			//Right
			8,9,10,
			10,11,8,

			//Left
			//12,14,13,
			//14,12,15,

			//Top
			16,17,18,
			18,19,16,

			//Bottom
			//20,22,21,
			//22,20,23,
			});

		return model;
	}

	static void* DimensionPTR;
	static void* serverPTR;

	BlockID ID = NULL;

	BlockType* Properties = nullptr;

	BlockTexture* Texture = nullptr;

	std::string BlockName = "";
};

__declspec(selectany) void* Block::DimensionPTR = nullptr;
__declspec(selectany) void* Block::serverPTR = nullptr;