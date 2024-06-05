#pragma once
#include <GLFW/glfw3.h>
#include "../../../RenderEngine/GUI/TextRenderer.h"

class DebugScreen {
private:
	GLFWwindow* window = nullptr;
	TextRenderer renderer;
	float fps;
	int VRAMUsageMB = 0;
	glm::vec3 Position = glm::vec3(0.f, 0.f, 0.f);
	void CreateTextComponent(std::string name, glm::vec2 Position) {

	}

public:
	void Render() {
		renderer.RenderFont();
	}

	void Update() {
		renderer.ConstructBuffer();
	}

	void EditText(std::string name, const char* c) {
		EditText(name, std::string(c));
	}

	void EditText(std::string name, std::string text) {
		renderer.EditFontText(name, text);
	}

	void Initialize(GLFWwindow* w) {
		window = w;
		renderer.InitializeTextRenderer(w);
		glm::vec3 clr = glm::vec3(0.2f, 0.2f, 0.2f);
		float a = 0.5;

		RenderableFont VersionFont;
		VersionFont.FontSize(0.05f);
		VersionFont.SetText(std::string("Version 1.2.1A"));
		VersionFont.SetPosition(glm::vec2(-1.f, 0.95f));
		VersionFont.AddBackground(clr, a);

		for (int i = 1; i <= 10; i++) {
			RenderableFont Stat;
			Stat.FontSize(0.04f);
			Stat.SetText(std::string("N/A"));
			Stat.SetPosition(glm::vec2(-1.f, 0.95f - 0.05f * i));
			Stat.AddBackground(clr, a);
			Stat.setBackgroundPadding(0.005f, 0.0f);
			renderer.InsertFontObject("Stat"+std::to_string(i), Stat);
		}

		renderer.InsertFontObject("Title", VersionFont);

		Update();

	}

};