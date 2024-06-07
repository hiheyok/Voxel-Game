#include "ItemTextureAtlas.h"

void ItemTextureAtlas::RenderBlockItem(Item item) {
	FramebufferSingleBlockRender.bindFBO();
	BlockItemRenderer.RenderBlock(item);
	FramebufferSingleBlockRender.unbindFBO();
}

void ItemTextureAtlas::StitchTexture(int index, int ItemID) {
	int ratio = AtlasSize / IndividualSize;
	float xCoord = (index % ratio);
	float yCoord = floor((float)index / (float)ratio);

	//Normalize

	xCoord = xCoord / (float)(ratio);
	yCoord = yCoord / (float)(ratio);

	float Size = 2 / (float)(ratio);

	//Map to NDC

	xCoord = -(xCoord * 2.f - 1.f);
	yCoord = -(yCoord * 2.f - 1.f);

	//Insert Data
	float vertices[] = {
		xCoord - 0.f	,yCoord - 0.f,	0.f, 1.f,
		xCoord - Size	,yCoord - 0.f,	1.f, 1.f,
		xCoord - Size	,yCoord - Size,	1.f, 0.f,
		xCoord - 0.f	,yCoord - Size,	0.f, 0.f,
	};

	uint32_t indices[] = {
		0, 1, 2,
		2, 3, 0
	};

	ItemUVMapping UVMAP;
	UVMAP.UV_1 = glm::vec2((xCoord + 1.f) * 0.5f, (yCoord + 1.f) * 0.5f);
	UVMAP.UV_2 = glm::vec2((xCoord - Size + 1.f) * 0.5f, (yCoord - Size + 1.f) * 0.5f);

	UVMAP.UV_1.y = 1.f - UVMAP.UV_1.y;
	UVMAP.UV_2.y = 1.f - UVMAP.UV_2.y;

	ItemsUVMap[ItemID] = UVMAP;

	VBO.InsertData(sizeof(vertices), vertices, GL_STATIC_DRAW);
	EBO.InsertData(sizeof(indices), indices, GL_STATIC_DRAW);
	//Render
	AtlasFramebuffer.bindFBO();
	StitchingShader.bindTexture2D(0, FramebufferSingleBlockRender.texture, "ItemTexture");

	glEnable(GL_BLEND);

	VAO.Bind();
	EBO.Bind();
	glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(uint32_t), GL_UNSIGNED_INT, 0);
	VAO.Unbind();
	EBO.Unbind();

	glDisable(GL_BLEND);
	AtlasFramebuffer.unbindFBO();
	//Clear Item FB
	FramebufferSingleBlockRender.clear();
	FramebufferSingleBlockRender.genBuffer(FramebufferSingleBlockRender.sx, FramebufferSingleBlockRender.sy, 1.f);
}

void ItemTextureAtlas::Initialize(int AtlasItemSize, int IndividualItemSize) {
	StitchingShader.init("assets/shaders/ItemRender/AtlasStitchVert.glsl", "assets/shaders/ItemRender/AtlasStitchFrag.glsl");

	IndividualSize = IndividualItemSize;
	AtlasSize = AtlasItemSize;

	AtlasFramebuffer.genBuffer(AtlasSize, AtlasSize, 1, GL_RGBA);
	FramebufferSingleBlockRender.genBuffer(IndividualSize, IndividualSize, 2, GL_RGBA);

	Atlas.width = AtlasSize;
	Atlas.height = AtlasSize;
	Atlas.textureID = AtlasFramebuffer.texture;

	IndividualItem.width = IndividualSize;
	IndividualItem.height = IndividualSize;
	IndividualItem.textureID = FramebufferSingleBlockRender.texture;

	VBO.SetType(GL_ARRAY_BUFFER);
	EBO.SetType(GL_ELEMENT_ARRAY_BUFFER);

	VBO.SetUsage(GL_STATIC_DRAW);
	EBO.SetUsage(GL_STATIC_DRAW);

	VAO.GenArray();
	VBO.GenBuffer();
	EBO.GenBuffer();

	VAO.Bind();
	VBO.Bind();
	VAO.EnableAttriPTR(0, 2, GL_FLOAT, GL_FALSE, 4, 0);
	VAO.EnableAttriPTR(1, 2, GL_FLOAT, GL_FALSE, 4, 2);
	VAO.Unbind();
	VBO.Unbind();

	BlockItemRenderer.Initialize();
}

void ItemTextureAtlas::AddItem(Item item) {
	Offsets[item.Properties.ID] = Offsets.size();
	RenderBlockItem(item);
	StitchTexture(Offsets.size() - 1, item.Properties.ID);
}