#pragma once

#include <unordered_map>
#include "../OpenGL/Shader/Shader.h"
#include "../OpenGL/Buffers/Buffer.h"
#include <vector>
#include "GUIObject.h"
#include "GUISet.h"

class GUI {
public:

	void Initialize(GLFWwindow* win) {
		window = win;
		shader.init("assets/shaders/GUI/GUIVert.glsl", "assets/shaders/GUI/GUIFrag.glsl");
	}

	int AddGUI(std::string Name, GUISet set) {
		GUIs.emplace_back(set);
		return GUIs.size() - 1;
	}

	GUISet& EditGUISet(int GUIIndex) {
		return GUIs[GUIIndex];
	}

	void PrepareRenderer() {
		for (auto& gui : GUIs) {
			gui.PrepareRenderer();
		}

		
	}

	void Render() {
		Update();
		SetupDrawCalls();

		shader.use();
		for (auto& gui : GUIs) {
			shader.bindTexture2D(0, gui.GUITexture.get(), "GUITexture");
			for (int i = 0; i < gui.NumOfRenderableObjects; i++) {
			//	std::cout << gui.Textures[i]->get() << "\n";
				
				gui.VAOs[i].Bind();
				gui.EBOs[i].Bind();
				glDrawElements(GL_TRIANGLES, gui.VBOSize[i], GL_UNSIGNED_INT, 0);
				gui.EBOs[i].Unbind();
				gui.VAOs[i].Unbind();
			}
			
		}
		
	}

private:

	void SetupDrawCalls() {
		glEnable(GL_BLEND);
		glDisable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);
		glDisable(GL_CULL_FACE);
	}

	void Update() {

		int Height, Width;
		glfwGetWindowSize(window, &Width, &Height);

		shader.use();
		shader.setFloat("AspectRatio", ((float)Height)/((float)Width));
	}

	std::vector<GUISet> GUIs = {};

	Shader shader;
	GLFWwindow* window;

	//bool isDirty = false;
};