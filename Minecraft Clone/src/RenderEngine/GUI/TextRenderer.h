#pragma once
#include "Font.h"
#include <unordered_map>
#include "../OpenGL/Shader/Shader.h"
#include "../OpenGL/Buffers/Buffer.h"
#include "../OpenGL/Texture/Types/Texture2D.h"
#include <exception>
#include "../../Utils/LogUtils.h"

class TextRenderer {
private:
	std::unordered_map<std::string, RenderableFont> FontMap;
	Shader FontShader;
	Buffer VBO;
	VertexArray VAO;
	Texture2D FontTexture;
	GLFWwindow* window = nullptr;

	std::vector<float> Vertices = {};
	
	int VerticesCount = 0;

public:
	void InitializeTextRenderer(GLFWwindow* w) {
		FontShader.init("assets/shaders/Font/FontVert.glsl", "assets/shaders/Font/FontFrag.glsl");
		VBO.GenBuffer();
		VAO.GenArray();

		VBO.SetType(GL_ARRAY_BUFFER);
		VBO.SetUsage(GL_STATIC_DRAW);

		VAO.Bind();
		VBO.Bind();
		VAO.EnableAttriPTR(0, 2, GL_FLOAT, GL_FALSE, 11, 0);
		VAO.EnableAttriPTR(1, 2, GL_FLOAT, GL_FALSE, 11, 2);
		VAO.EnableAttriPTR(2, 2, GL_FLOAT, GL_FALSE, 11, 4);
		VAO.EnableAttriPTR(3, 2, GL_FLOAT, GL_FALSE, 11, 6);
		VAO.EnableAttriPTR(4, 3, GL_FLOAT, GL_FALSE, 11, 8);
		VAO.Unbind();
		VBO.Unbind();
		RawTextureData RawTexture;
		RawTexture.Load("assets/textures/font/ascii.png");
		FontTexture.Load(RawTexture);
		FontShader.bindTexture2D(0, FontTexture.get(), "FontTexture");
		window = w;
		Logger.LogDebug("Font Renderer", "Initialized font renderer");
	}

	void InsertFontObject(std::string name ,RenderableFont font) {
		if (FontMap.count(name)) {
			throw std::exception(std::string("Font with the name " + name + " already exist!").c_str());
		}

		FontMap[name] = font;
	}

	void RemoveFontObject(std::string name) {
		if (!FontMap.count(name)) {
			throw std::exception(std::string("Font with the name " + name + " doesn't exist! Cannot remove.").c_str());
		}

		FontMap.erase(name);
	}

	void EditFontText(std::string name, std::string text) {
		FontMap[name].SetText(text);
	}

	void ConstructBuffer() {
		Vertices.clear();

		for (auto& Font : FontMap) {
			std::vector<float> fontVertices = Font.second.getVertices();

			Vertices.insert(Vertices.end(), fontVertices.begin(), fontVertices.end());
		}

		VBO.InsertData(Vertices.size() * sizeof(Vertices[0]), Vertices.data(), GL_STATIC_DRAW);
		VerticesCount = Vertices.size() / (9);
	}

	void Prepare() {
		FontShader.use();
		FontShader.bindTexture2D(0, FontTexture.get(), "FontTexture");

		int Height, Width;
		glfwGetWindowSize(window, &Width, &Height);

		FontShader.setFloat("AspectRatio", ((float)Height) / ((float)Width));
	}

	void RenderFont() {
		Prepare();
		FontShader.use();
		VAO.Bind();
		glDrawArrays(GL_TRIANGLES, 0, VerticesCount);
	}
};