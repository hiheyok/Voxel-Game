#pragma once
#include <unordered_map>
#include "../OpenGL/Shader/Shader.h"
#include "../OpenGL/Buffers/Buffer.h"
#include <vector>
#include "GUIObject.h"
#include "../OpenGL/Texture/Types/Texture2D.h"
#include "../../Level/Typenames.h"

class GUISet {
private:
	std::vector<GUIElement> elements_;

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

	GUISet() {}

	void Initialize() {
		
	}

	void Clean() {
		for (auto& vao : VAOs)
			vao.Delete();
		for (auto& vbo : VBOs)
			vbo.Delete();
		for (auto& ebo : EBOs)
			ebo.Delete();
	}

	void AddGUIElementNorm(std::string Name, std::string Text, glm::vec2 Size, glm::vec2 Position, glm::vec2 UV_P0, glm::vec2 UV_P1) {

		if (!is_initialized_) {
			Initialize();
			is_initialized_ = true;
		}

		if (GUIElementIndex.find(Name) == GUIElementIndex.end()) {
			elements_.emplace_back(Text, Size, Position);
			elements_.back().buffer_index_ = AddRenderingObj();
			elements_.back().uv_p0_ = UV_P0;
			elements_.back().uv_p1_ = UV_P1;
			GUIElementIndex.emplace(Name, elements_.size() - 1);
			NumOfRenderableObjects++;
			isDirty = true;
		}
		else {
			Logger.LogError("GUI", "Element " + Name + " already exist!");
		}
	}

	void AddGUIElement(std::string Name, std::string Text, glm::vec2 Size, glm::vec2 Position, glm::vec2 UV_P0, glm::vec2 UV_P1) {

		if (!is_initialized_) {
			Initialize();
			is_initialized_ = true;
		}

		UV_P0.x = UV_P0.x / (float)GUITexture.width_;
		UV_P1.x = UV_P1.x / (float)GUITexture.width_;

		UV_P0.y = UV_P0.y / (float)GUITexture.height_;
		UV_P1.y = UV_P1.y / (float)GUITexture.height_;

		if (GUIElementIndex.find(Name) == GUIElementIndex.end()) {
			elements_.emplace_back(Text, Size, Position);
			elements_.back().buffer_index_ = AddRenderingObj();
			elements_.back().uv_p0_ = UV_P0;
			elements_.back().uv_p1_ = UV_P1;
			GUIElementIndex.emplace(Name, elements_.size() - 1);
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
			elements_[Index].location_ = Position;
			isDirty = true;
		}
		else {
			Logger.LogError("GUI", "Element " + Name + " doesn't exist!");
		}
	}

	void EditElementUVNorm(std::string Name, glm::vec2 UV0, glm::vec2 UV1) {
		if (GUIElementIndex.find(Name) != GUIElementIndex.end()) {
			int Index = GUIElementIndex[Name];
			elements_[Index].uv_p0_ = UV0;
			elements_[Index].uv_p1_ = UV1;
			isDirty = true;
		}
		else {
			Logger.LogError("GUI", "Element " + Name + " doesn't exist!");
		}
	}

	void SetGUITexture(std::string file) {
		GUITexture = Texture2D(RawTextureData(file.c_str()));
	}

	void SetGUITexture(GLuint texture_id_, size_t x, size_t y) {
		GUITexture.textureID = texture_id_;
		GUITexture.width_ = x;
		GUITexture.height_ = y;
	}

	void PrepareRenderer() {
		if (!isDirty) {
			return;
		}
		isDirty = false;

		for (auto& e : elements_) {
			
			GUIElement::GUIVertices GUIData = e.GetVertices();
			int BufferIndex = e.buffer_index_;
			VBOs[BufferIndex].InsertData(GUIData.vertices_.size() * sizeof(float), GUIData.vertices_.data(), GL_STATIC_DRAW);
			EBOs[BufferIndex].InsertData(GUIData.indices_.size() * sizeof(unsigned int), GUIData.indices_.data(), GL_STATIC_DRAW);
			VBOSize[BufferIndex] = GUIData.indices_.size();
		}

		
	}

	// TODO: Rename
	std::vector<VertexArray> VAOs;
	std::vector<Buffer> VBOs;
	std::vector<Buffer> EBOs;
	std::vector<int> VBOSize;

	int NumOfRenderableObjects = 0;

	
	FastHashMap<std::string, int> GUIElementIndex;
	bool isDirty = true;

	Texture2D GUITexture;

	bool is_initialized_ = false;
};