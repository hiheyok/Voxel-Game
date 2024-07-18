#include "DebugScreen.h"

void DebugScreen::Render() {
	renderer.RenderFont();
}

void DebugScreen::Update() {
	renderer.ConstructBuffer();
}

void DebugScreen::EditText(std::string name, const char* c) {
	EditText(name, std::string(c));
}

void DebugScreen::EditText(std::string name, std::string text) {
	renderer.EditFontText(name, text);
}

void DebugScreen::Initialize(GLFWwindow* w) {
	window = w;
	renderer.InitializeTextRenderer(w);
	glm::vec3 clr = glm::vec3(0.2f, 0.2f, 0.2f);
	float a = 0.5;

	RenderableFont VersionFont;
	VersionFont.FontSize(0.05f);
	VersionFont.SetText(std::string("Version 1.3.0A"));
	VersionFont.SetPosition(glm::vec2(-1.f, 0.95f));
	VersionFont.AddBackground(clr, a);

	for (int i = 1; i <= 10; i++) {
		RenderableFont Stat;
		Stat.FontSize(0.04f);
		Stat.SetText(std::string("N/A"));
		Stat.SetPosition(glm::vec2(-1.f, 0.95f - 0.05f * i));
		Stat.AddBackground(clr, a);
		Stat.setBackgroundPadding(0.005f, 0.0f);
		renderer.InsertFontObject("Stat" + std::to_string(i), Stat);
	}

	renderer.InsertFontObject("Title", VersionFont);

	Update();
}