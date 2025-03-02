#pragma once
#include "Font.h"
#include <unordered_map>
#include "../OpenGL/Shader/Shader.h"
#include "../OpenGL/Buffers/Buffer.h"
#include "../OpenGL/Texture/Types/Texture2D.h"
#include <exception>
#include "../../Utils/LogUtils.h"
// TODO: Rename
class TextRenderer {
private:
	FastHashMap<std::string, RenderableFont> FontMap;
	Shader FontShader, backgroundShader;
	Buffer VBO, backgroundVBO;
	VertexArray VAO, backgroundVAO;
	Texture2D FontTexture;
	GLFWwindow* window = nullptr;

	std::vector<float> Vertices = {};
	std::vector<float> VerticesBackground = {};
	
	size_t VerticesCount = 0;
	size_t BackgroundVertCount = 0;

public:
	void InitializeTextRenderer(GLFWwindow* w) {
		//Initialize shaders
		FontShader.init("assets/shaders/Font/FontVert.glsl", "assets/shaders/Font/FontFrag.glsl");
		backgroundShader.init("assets/shaders/Font/FontBackgroundVert.glsl", "assets/shaders/Font/FontBackgroundFrag.glsl");

		//Setup buffer for text rendering
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

		//Setup buffer for background rendering
		backgroundVBO.GenBuffer();
		backgroundVAO.GenArray();

		backgroundVBO.SetType(GL_ARRAY_BUFFER);
		backgroundVBO.SetUsage(GL_STATIC_DRAW);

		backgroundVAO.Bind();
		backgroundVBO.Bind();
		backgroundVAO.EnableAttriPTR(0, 2, GL_FLOAT, GL_FALSE, 8, 0);
		backgroundVAO.EnableAttriPTR(1, 2, GL_FLOAT, GL_FALSE, 8, 2);
		backgroundVAO.EnableAttriPTR(2, 4, GL_FLOAT, GL_FALSE, 8, 4);
		backgroundVAO.Unbind();
		backgroundVBO.Unbind();

		//Load textures
		RawTextureData RawTexture;
		RawTexture.Load("assets/minecraft/textures/font/ascii.png");
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
		//Fill text buffer
		Vertices.clear();

		for (auto& Font : FontMap) {
			std::vector<float> fontVertices = Font.second.GetVertices();

			Vertices.insert(Vertices.end(), fontVertices.begin(), fontVertices.end());
		}

		VBO.InsertData(Vertices.size() * sizeof(Vertices[0]), Vertices.data(), GL_STATIC_DRAW);
		VerticesCount = Vertices.size() / 11;

		//Fill background buffer
		VerticesBackground.clear();

		for (auto& Font : FontMap) {
			if (!Font.second.background_) continue;

			std::vector<float> backgroundVertices = Font.second.GetBackgroundVertices();

			VerticesBackground.insert(VerticesBackground.end(), backgroundVertices.begin(), backgroundVertices.end());
		}

		backgroundVBO.InsertData(VerticesBackground.size() * sizeof(VerticesBackground[0]), VerticesBackground.data(), GL_STATIC_DRAW);
		BackgroundVertCount = VerticesBackground.size() / 8;
	}

	void Prepare() {
		FontShader.use();
		FontShader.bindTexture2D(0, FontTexture.get(), "FontTexture");

		int Height, Width;
		glfwGetWindowSize(window, &Width, &Height);

		FontShader.setFloat("AspectRatio", ((float)Height) / ((float)Width));

		backgroundShader.use();
		backgroundShader.setFloat("AspectRatio", ((float)Height) / ((float)Width));
	}

	void RenderFont() {
		Prepare();
		backgroundShader.use();
		backgroundVAO.Bind();
		glDrawArrays(GL_TRIANGLES, 0, BackgroundVertCount);

		FontShader.use();
		VAO.Bind();
		glDrawArrays(GL_TRIANGLES, 0, VerticesCount);
	}
};