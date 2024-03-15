#pragma once
#include <unordered_map>
#include "../OpenGL/Shader/Shader.h"
#include "../OpenGL/Buffers/Buffer.h"
#include <vector>
#include "GUIObject.h"
#include "../OpenGL/Texture/Types/Texture2D.h"

class GUISet {
private:
	std::vector<GUIElement> Elements;

	int AddRenderingObj() {
		VAOs.emplace_back();
		EBOs.emplace_back();
		VBOs.emplace_back();
		VAOs.back().GenArray();
		VBOs.back().GenBuffer();
		EBOs.back().GenBuffer();

		VBOs.back().SetType(GL_ARRAY_BUFFER);
		EBOs.back().SetType(GL_ELEMENT_ARRAY_BUFFER);

		VBOs.back().SetUsage(GL_STATIC_DRAW);
		EBOs.back().SetUsage(GL_STATIC_DRAW);

		VAOs.back().Bind();
		VBOs.back().Bind();
		VAOs.back().EnableAttriPTR(0, 2, GL_FLOAT, GL_FALSE, 4, 0);
		VAOs.back().EnableAttriPTR(1, 2, GL_FLOAT, GL_FALSE, 4, 2);
		VBOs.back().Unbind();
		VAOs.back().Unbind();

		VBOSize.emplace_back(0);
		return VAOs.size() - 1;
	}

public:

	GUISet() {
	}


	void Initialize() {
		
	}

	void Clean() {
		for (auto& VAO : VAOs)
			VAO.Delete();
		for (auto& VBO : VBOs)
			VBO.Delete();
		for (auto& EBO : EBOs)
			EBO.Delete();
	}

	void AddGUIElementNorm(std::string Name, std::string Text, glm::vec2 Size, glm::vec2 Position, glm::vec2 UV_P0, glm::vec2 UV_P1) {

		if (!isInitialized) {
			Initialize();
			isInitialized = true;
		}

		if (GUIElementIndex.find(Name) == GUIElementIndex.end()) {
			Elements.emplace_back(Text, Size, Position);
			Elements.back().BufferIndex = AddRenderingObj();
			Elements.back().UV_P0 = UV_P0;
			Elements.back().UV_P1 = UV_P1;
			GUIElementIndex.emplace(Name, Elements.size() - 1);
			NumOfRenderableObjects++;
			isDirty = true;
		}
		else {
			Logger.LogError("GUI", "Element " + Name + " already exist!");
		}
	}

	void AddGUIElement(std::string Name, std::string Text, glm::vec2 Size, glm::vec2 Position, glm::vec2 UV_P0, glm::vec2 UV_P1) {

		if (!isInitialized) {
			Initialize();
			isInitialized = true;
		}

		UV_P0.x = UV_P0.x / (float)GUITexture.width;
		UV_P1.x = UV_P1.x / (float)GUITexture.width;

		UV_P0.y = UV_P0.y / (float)GUITexture.height;
		UV_P1.y = UV_P1.y / (float)GUITexture.height;

		if (GUIElementIndex.find(Name) == GUIElementIndex.end()) {
			Elements.emplace_back(Text, Size, Position);
			Elements.back().BufferIndex = AddRenderingObj();
			Elements.back().UV_P0 = UV_P0;
			Elements.back().UV_P1 = UV_P1;
			GUIElementIndex.emplace(Name, Elements.size() - 1);
			NumOfRenderableObjects++;
			isDirty = true;
		}
		else {
			Logger.LogError("GUI", "Element " + Name + " already exist!");
		}
	}

	void EditElementPosition(std::string Name, glm::vec2 Position) {
		if (GUIElementIndex.find(Name) != GUIElementIndex.end()) {
			int Index = GUIElementIndex[Name];
			Elements[Index].Location = Position;
			isDirty = true;
		}
		else {
			Logger.LogError("GUI", "Element " + Name + " doesn't exist!");
		}
	}

	void EditElementUVNorm(std::string Name, glm::vec2 UV0, glm::vec2 UV1) {
		if (GUIElementIndex.find(Name) != GUIElementIndex.end()) {
			int Index = GUIElementIndex[Name];
			Elements[Index].UV_P0 = UV0;
			Elements[Index].UV_P1 = UV1;
			isDirty = true;
		}
		else {
			Logger.LogError("GUI", "Element " + Name + " doesn't exist!");
		}
	}

	void SetGUITexture(std::string file) {
		GUITexture = Texture2D(RawTextureData(file.c_str()));
	}

	void SetGUITexture(GLuint TextureID, size_t x, size_t y) {
		GUITexture.textureID = TextureID;
		GUITexture.width = x;
		GUITexture.height = y;
	}

	void PrepareRenderer() {
		if (!isDirty) {
			return;
		}
		isDirty = false;

		for (auto& e : Elements) {
			
			GUIElement::GUIVertices GUIData = e.GetVertices();
			int BufferIndex = e.BufferIndex;
			VBOs[BufferIndex].InsertData(GUIData.Vertices.size() * sizeof(float), GUIData.Vertices.data(), GL_STATIC_DRAW);
			EBOs[BufferIndex].InsertData(GUIData.Indices.size() * sizeof(unsigned int), GUIData.Indices.data(), GL_STATIC_DRAW);
			VBOSize[BufferIndex] = GUIData.Indices.size();
		}

		
	}


	std::vector<VertexArray> VAOs;
	std::vector<Buffer> VBOs;
	std::vector<Buffer> EBOs;
	std::vector<int> VBOSize;

	int NumOfRenderableObjects = 0;

	
	std::unordered_map<std::string, int> GUIElementIndex;
	bool isDirty = true;

	Texture2D GUITexture;

	bool isInitialized = false;
};