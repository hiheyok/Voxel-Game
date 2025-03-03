#include "DebugScreen.h"

void DebugScreen::Render() {
	renderer_.RenderFont();
}

void DebugScreen::Update() {
	renderer_.ConstructBuffer();
}

void DebugScreen::EditText(std::string name, const char* c) {
	EditText(name, std::string(c));
}

void DebugScreen::EditText(std::string name, std::string text) {
	renderer_.EditFontText(name, text);
}

void DebugScreen::Initialize(GLFWwindow* w) {
	window_ = w;
	renderer_.InitializeTextRenderer(w);
	glm::vec3 clr = glm::vec3(0.2f, 0.2f, 0.2f);
	float a = 0.5;

	RenderableFont versionFont;
	versionFont.FontSize(0.05f);
	versionFont.SetText(std::string("Version 1.3.0A"));
	versionFont.SetPosition(glm::vec2(-1.f, 0.95f));
	versionFont.AddBackground(clr, a);

	for (int i = 1; i <= 10; i++) {
		RenderableFont stat;
		stat.FontSize(0.04f);
		stat.SetText(std::string("N/A"));
		stat.SetPosition(glm::vec2(-1.f, 0.95f - 0.05f * i));
		stat.AddBackground(clr, a);
		stat.setBackgroundPadding(0.005f, 0.0f);
		renderer_.InsertFontObject("Stat" + std::to_string(i), stat);
	}

	renderer_.InsertFontObject("Title", versionFont);

	Update();
}