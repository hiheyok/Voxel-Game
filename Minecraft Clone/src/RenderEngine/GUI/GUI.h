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
		window_ = win;
		shader_.Init("assets/shaders/GUI/GUIVert.glsl", "assets/shaders/GUI/GUIFrag.glsl");
	}

	size_t AddGUI(std::string Name, GUISet set) {
		guis_.emplace_back(set);
		return guis_.size() - 1;
	}

	GUISet& EditGUISet(size_t GUIIndex) {
		return guis_[GUIIndex];
	}

	void PrepareRenderer() {
		for (auto& gui : guis_) {
			gui.PrepareRenderer();
		}

		
	}

	void Render() {
		Update();
		SetupDrawCalls();

		shader_.Use();
		for (auto& gui : guis_) {
			shader_.BindTexture2D(0, gui.gui_texture_.get(), "GUITexture");
			for (int i = 0; i < gui.num_of_renderable_objects_; i++) {
			//	std::cout << gui.Textures[i]->get() << "\n";
				
				gui.vaos_[i].Bind();
				gui.ebos_[i].Bind();
				glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(gui.vbo_size_[i]), GL_UNSIGNED_INT, 0);
				gui.ebos_[i].Unbind();
				gui.vaos_[i].Unbind();
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
		glfwGetWindowSize(window_, &Width, &Height);

		shader_.Use();
		shader_.SetFloat("AspectRatio", ((float)Height)/((float)Width));
	}

	std::vector<GUISet> guis_ = {};

	Shader shader_;
	GLFWwindow* window_ = nullptr;

	//bool isDirty = false;
};