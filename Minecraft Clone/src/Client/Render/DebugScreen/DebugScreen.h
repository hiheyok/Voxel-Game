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

		RenderableFont VersionFont;
		VersionFont.FontSize(0.0625f);
		VersionFont.SetText(std::string("Version 1.2.1A"));
		VersionFont.SetPosition(glm::vec2(-1.f, 0.9f));

		RenderableFont FPSFont;
		FPSFont.FontSize(0.04f);
		FPSFont.SetText(std::string("0 FPS"));
		FPSFont.SetPosition(glm::vec2(-1.f, 0.85f));

		RenderableFont PositionFont;
		PositionFont.FontSize(0.04f);
		PositionFont.SetText(std::string("XYZ: " + std::to_string(0.0) + "/" + std::to_string(0.0) + "/" + std::to_string(0.0)));
		PositionFont.SetPosition(glm::vec2(-1.f, 0.8f));

		RenderableFont VRAMUsage;
		VRAMUsage.FontSize(0.04f);
		VRAMUsage.SetText(std::string("VRAM Usage: 0 MB"));
		VRAMUsage.SetPosition(glm::vec2(-1.f, 0.75f));

		RenderableFont VRAMFragmentationRate;
		VRAMFragmentationRate.FontSize(0.04f);
		VRAMFragmentationRate.SetText(std::string("VRAM Fragmentation: 0 %"));
		VRAMFragmentationRate.SetPosition(glm::vec2(-1.f, 0.7f));

		RenderableFont MeshStats;
		MeshStats.FontSize(0.04f);
		MeshStats.SetText(std::string("N/A"));
		MeshStats.SetPosition(glm::vec2(-1.f, 0.65f));

		RenderableFont EntityCount;
		EntityCount.FontSize(0.04f);
		EntityCount.SetText(std::string("N/A"));
		EntityCount.SetPosition(glm::vec2(-1.f, 0.60f));

		RenderableFont EventQueueSize;
		EventQueueSize.FontSize(0.04f);
		EventQueueSize.SetText(std::string("N/A"));
		EventQueueSize.SetPosition(glm::vec2(-1.f, 0.55f));

		RenderableFont ServerTick;
		ServerTick.FontSize(0.04f);
		ServerTick.SetText(std::string("N/A"));
		ServerTick.SetPosition(glm::vec2(-1.f, 0.50f));

		RenderableFont Stat9;
		Stat9.FontSize(0.04f);
		Stat9.SetText(std::string("N/A"));
		Stat9.SetPosition(glm::vec2(-1.f, 0.45f));

		RenderableFont Stat10;
		Stat10.FontSize(0.04f);
		Stat10.SetText(std::string("N/A"));
		Stat10.SetPosition(glm::vec2(-1.f, 0.40f));

		renderer.InsertFontObject("Title", VersionFont);
		renderer.InsertFontObject("Stat1", FPSFont);
		renderer.InsertFontObject("Stat2", PositionFont);
		renderer.InsertFontObject("Stat3", VRAMUsage);
		renderer.InsertFontObject("Stat4", VRAMFragmentationRate);
		renderer.InsertFontObject("Stat5", MeshStats);
		renderer.InsertFontObject("Stat6", EntityCount);
		renderer.InsertFontObject("Stat7", EventQueueSize);
		renderer.InsertFontObject("Stat8", ServerTick);

		renderer.InsertFontObject("Stat9", Stat9);
		renderer.InsertFontObject("Stat10", Stat10);
		Update();

	}

};