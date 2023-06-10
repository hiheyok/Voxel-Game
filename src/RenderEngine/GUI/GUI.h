#pragma once
#ifndef IGUI_H
#define IGUI_H
#define _CRTDBG_MAP_ALLOC
#include <unordered_map>
#include "../OpenGL/Shader/Shader.h"
#include "../OpenGL/Texture/Types/Texture2D.h"
#include "../OpenGL/Framebuffer/Framebuffer.h"
#include <chrono>
#include <vector>



struct GUI_Data {
	std::string name;
	fvec2 p0, p1, p2, p3 = fvec2(0.0f, 0.0f);
	fvec2 t0, t1, t2, t3 = fvec2(0.0f, 0.0f);
	fvec3 color;
	bool clickable = false;
	bool isSlider = false;
	std::string text;
	int Slider_MAX = 0;
	int Slider_MIN = 0;
	int Slider_CVAL = 0;
	std::vector<std::string> guiDel;
	std::vector<std::string> guiAdd;
	bool render = false;
	int gevent = NULL;
	unsigned int renderstep;
};

struct LETTER {
	fvec2 p0 = fvec2(0.0f, 0.0f), p1 = fvec2(0.0f, 0.0f), p2 = fvec2(0.0f, 0.0f), p3 = fvec2(0.0f,0.0f);
	fvec2 t0 = fvec2(0.0f, 0.0f), t1 = fvec2(0.0f, 0.0f), t2 = fvec2(0.0f, 0.0f), t3 = fvec2(0.0f, 0.0f);
};

struct string_rendering_data {
	std::vector<LETTER> string;
	float letterSize = 0.f;
	fvec3 color = fvec3(0.f,0.f,0.f);
	bool render = false;
};


class IGUI {
public:


	void init(GLFWwindow* window_, std::vector<int>* CE);

	void addGUI(std::string name, fvec2 size, fvec2 pos, fvec3 color, fvec2 tpos0, fvec2 tpos1, bool clickable, int step);
	void addGUIresizedTexture(std::string name, fvec2 size, fvec2 pos, fvec3 color, fvec2 tpos0, fvec2 tpos1, fvec2 textureSize, bool clickable, int step);
	void addGUIresizedTexturewFont(std::string name, std::string str, fvec2 size, fvec2 pos, fvec3 color, fvec2 tpos0, fvec2 tpos1, fvec2 textureSize, bool clickable, int step);

	void addToRenderQueue(std::string name);
	void deleteFromRenderQueue(std::string name);

	void addToRenderQueue(std::vector<std::string> name);
	void deleteFromRenderQueue(std::vector<std::string> name);

	void Render();

	void update();

	void changeGUIcall(std::string name, std::vector<std::string> add, std::vector<std::string> del);
	void changeGUIEvent(std::string name, int EVENT);

	void configTexture(int renderStep, std::string tdir, int index);
	void configTexture(int renderStep, TexturedFrameBuffer fbo, int index);

	ivec4 getCharTextCoords(const char letter);

	ivec4 getPixelCoords(ivec2 TextureSize, ivec2 lu, ivec2 rd);
	ivec4 getPixelCoordsOffset(ivec2 TextureSize, ivec2 lu, ivec2 offset);

	GUI_Data* getGUI(std::string name);

	Shader guiShader[16]{};

	ivec2 MouseLeftButtonPressPos;
	ivec2 MouseRightButtonPressPos;
	ivec2 MousePos;

	std::vector<int>* CLIENT_EVENTS;
	GLFWwindow* window;

	bool clicked;

	int debug0, debug1, debug2, debug3;
	
	int RenderingStepsCount = 1;
	void prepareRenderer();
private:

	std::chrono::steady_clock::time_point t0 = std::chrono::high_resolution_clock::now();

	void clickDetection();

	

	void tick();

	Texture2D texture[16]{};
	Texture2D font;

	unsigned int VBO[16]{}, VAO[16]{}, EBO[16]{}, size[16]{};

	unsigned int fontVBO, fontVAO, fontEBO;

	size_t fontsize;

	std::unordered_map<std::string, GUI_Data> data;
	std::unordered_map<std::string, string_rendering_data> gui_string;
	std::vector<std::string> tickable;



};

#endif