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

	void EditText(std::string name, std::string text) {
		renderer.EditFontText(name, text);
	}

	void Initialize(GLFWwindow* w) {
		window = w;
		renderer.InitializeTextRenderer(w);

		RenderableFont VersionFont;
		VersionFont.FontSize(0.0625f);
		VersionFont.SetText(std::string("Version 1.2A"));
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

		renderer.InsertFontObject("Version", VersionFont);
		renderer.InsertFontObject("FPS", FPSFont);
		renderer.InsertFontObject("Position", PositionFont);
		renderer.InsertFontObject("VRAMUsage", VRAMUsage);
		renderer.InsertFontObject("VRAMFragmentationRate", VRAMFragmentationRate);
		renderer.InsertFontObject("MeshStats", MeshStats);
		renderer.InsertFontObject("EntityCount", EntityCount);
		renderer.InsertFontObject("EventQueueSize", EventQueueSize);
		renderer.InsertFontObject("ServerTick", ServerTick);
		Update();

	}

};