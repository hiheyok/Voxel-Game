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
		Textures.emplace_back();
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

		for (auto& Tex : Textures)
			delete Tex;
	}

	void AddGUIElement(std::string Name, std::string Text, glm::vec2 Size, glm::vec2 Position) {

		if (!isInitialized) {
			Initialize();
			isInitialized = true;
		}

		if (GUIElementIndex.find(Name) == GUIElementIndex.end()) {
			Elements.emplace_back(Text, Size, Position);
			Elements.back().BufferIndex = AddRenderingObj();
			GUIElementIndex.emplace(Name, Elements.size() - 1);
			NumOfRenderableObjects++;
		//	PrepareRenderer()
			isDirty = true;
		}
		else {
			Logger.LogError("GUI", "Element " + Name + " already exist!");
		}
	}

	void EditGUIElementTexture(std::string Name, std::string file, glm::vec2 UV_P0, glm::vec2 UV_P1) {
		//Load textures
		
		Texture2D* tex = new Texture2D();
		RawTextureData RawData;
		RawData.Load(file.c_str());
		tex->Gen();
		tex->Load(RawData);

		int index = GUIElementIndex[Name];
		Textures[index] = (tex);
		Elements[index].UV_P0 = UV_P0;
		Elements[index].UV_P1 = UV_P1;
		

	}

	void PrepareRenderer() {
		if (!isDirty) {
			return;
		}

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

	std::vector<Texture2D*> Textures;

	bool isInitialized = false;
};