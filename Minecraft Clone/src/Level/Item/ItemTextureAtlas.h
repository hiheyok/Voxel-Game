#pragma once
#include "../../RenderEngine/OpenGL//Texture/Types/Texture2D.h"
#include "Item.h"
#include <unordered_map>
#include "../../RenderEngine/OpenGL/Framebuffer/Framebuffer.h"
#include "../../RenderEngine/ItemRender/BlockItemRender.h"
#include "../../RenderEngine/OpenGL/Buffers/Buffer.h"
#include "../../RenderEngine/OpenGL/Shader/Shader.h"

#include <glm/vec2.hpp>

struct ItemUVMapping {
	glm::vec2 UV_1;
	glm::vec2 UV_2;
};

class ItemTextureAtlas {
	Texture2D IndividualItem;

	VertexArray VAO;
	Buffer EBO;
	Buffer VBO;

	BlockItemRender BlockItemRenderer;
	Shader StitchingShader;
	int IndividualSize = 0;
	int AtlasSize = 0;
	TexturedFrameBuffer AtlasFramebuffer;
	TexturedFrameBuffer FramebufferSingleBlockRender;

	void RenderBlockItem(Item item);

	void StitchTexture(int index, int ItemID);

	
public:
	std::unordered_map<int, ItemUVMapping> ItemsUVMap;
	
	Texture2D Atlas;

	std::unordered_map<int, int> Offsets;
	
	void Initialize(int AtlasItemSize, int IndividualItemSize);

	void AddItem(Item item);
};

extern ItemTextureAtlas ItemAtlas;