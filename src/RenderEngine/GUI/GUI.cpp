#include "GUI.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../../Utils/LogUtils.h"
#define _CRTDBG_MAP_ALLOC

#define FONT 16

using namespace std;
using namespace glm;
using namespace chrono;

void IGUI::addGUI(string name, vec2 size, vec2 pos, vec3 color, vec2 tpos0, vec2 tpos1, bool clickable, int step) {

	GUI_Data gui;
	vec2 p0 = vec2(pos.x - size.x, pos.y - size.y);
	vec2 t0 = vec2(tpos0.x, tpos1.y);

	vec2 p1 = vec2(pos.x + size.x, pos.y - size.y);
	vec2 t1 = vec2(tpos1.x, tpos1.y);

	vec2 p2 = vec2(pos.x + size.x, pos.y + size.y);
	vec2 t2 = vec2(tpos1.x, tpos0.y);

	vec2 p3 = vec2(pos.x - size.x, pos.y + size.y);
	vec2 t3 = vec2(tpos0.x, tpos0.y);;

	gui.p0 = p0;
	gui.p1 = p1;
	gui.p2 = p2;
	gui.p3 = p3;
	gui.t0 = t0;
	gui.t1 = t1;
	gui.t2 = t2;
	gui.t3 = t3;
	gui.color.r = color.r / 255.0f;
	gui.color.g = color.g / 255.0f;
	gui.color.b = color.b / 255.0f;
	gui.name = name;
	gui.clickable = clickable;
	gui.renderstep = step - 1;
	data[name] = gui;
}

void IGUI::addGUIresizedTexture(std::string name, vec2 size, vec2 pos, vec3 color, vec2 tpos0, vec2 tpos1, vec2 textureSize, bool clickable, int step) {

	tpos0 = vec2(tpos0.x / textureSize.x, tpos0.y / textureSize.y);
	tpos1 = vec2(tpos1.x / textureSize.x, tpos1.y / textureSize.y);

	size = vec2((tpos1.x - tpos0.x) * size.x, (tpos1.y - tpos0.y) * size.y);

	GUI_Data gui;
	vec2 p0 = vec2(pos.x - size.x, pos.y - size.y);
	vec2 t0 = vec2(tpos0.x, tpos1.y);

	vec2 p1 = vec2(pos.x + size.x, pos.y - size.y);
	vec2 t1 = vec2(tpos1.x, tpos1.y);

	vec2 p2 = vec2(pos.x + size.x, pos.y + size.y);
	vec2 t2 = vec2(tpos1.x, tpos0.y);

	vec2 p3 = vec2(pos.x - size.x, pos.y + size.y);
	vec2 t3 = vec2(tpos0.x, tpos0.y);;

	gui.p0 = p0;
	gui.p1 = p1;
	gui.p2 = p2;
	gui.p3 = p3;
	gui.t0 = t0;
	gui.t1 = t1;
	gui.t2 = t2;
	gui.t3 = t3;
	gui.color.r = color.r / 255.0f;
	gui.color.g = color.g / 255.0f;
	gui.color.b = color.b / 255.0f;
	gui.name = name;
	gui.clickable = clickable;
	gui.renderstep = step - 1;
	data[name] = gui;
}

void IGUI::addGUIresizedTexturewFont(std::string name, std::string str, vec2 size, vec2 pos, vec3 color, vec2 tpos0, vec2 tpos1, vec2 textureSize, bool clickable, int step) {


	tpos0 = vec2(tpos0.x / textureSize.x, tpos0.y / textureSize.y);
	tpos1 = vec2(tpos1.x / textureSize.x, tpos1.y / textureSize.y);

	size = vec2((tpos1.x - tpos0.x) * size.x, (tpos1.y - tpos0.y) * size.y);

	GUI_Data gui;
	vec2 p0 = vec2(pos.x - size.x, pos.y - size.y);
	vec2 t0 = vec2(tpos0.x, tpos1.y);

	vec2 p1 = vec2(pos.x + size.x, pos.y - size.y);	
	vec2 t1 = vec2(tpos1.x, tpos1.y);

	vec2 p2 = vec2(pos.x + size.x, pos.y + size.y);	
	vec2 t2 = vec2(tpos1.x, tpos0.y);

	vec2 p3 = vec2(pos.x - size.x, pos.y + size.y);	
	vec2 t3 = vec2(tpos0.x, tpos0.y);;

	gui.p0 = p0;
	gui.p1 = p1;
	gui.p2 = p2;
	gui.p3 = p3;
	gui.t0 = t0;
	gui.t1 = t1;
	gui.t2 = t2;
	gui.t3 = t3;
	gui.color.r = color.r / 255.0f;
	gui.color.g = color.g / 255.0f;
	gui.color.b = color.b / 255.0f;
	gui.name = name;
	gui.clickable = clickable;
	gui.renderstep = step - 1;
	data[name] = gui;

	const char* text = str.c_str();

	string_rendering_data rtext;

	float fontsize = .4;

	size.y = size.y * fontsize;

	rtext.letterSize = size.y;

	for (int i = 0; i < (int)str.size(); i++) {
		char letter = text[i];
		vec4 textpos = getCharTextCoords(letter);
		tpos0 = vec2(textpos.x, textpos.w);
		tpos1 = vec2(textpos.z, textpos.y);
		

		double offset = ((float)i - ((float)str.size() / 2.0f)) * size.y * 2;

		float margin = .8;

		p0 = vec2(pos.x - size.y * margin + offset, pos.y - size.y * margin);	//lf bt
		t0 = tpos0;
		p1 = vec2(pos.x + size.y * margin + offset, pos.y - size.y * margin);	//rt bt
		t1 = vec2(tpos1.x, tpos0.y);
		p2 = vec2(pos.x + size.y * margin + offset, pos.y + size.y * margin);	//rt up
		t2 = vec2(tpos1.x, tpos1.y);
		p3 = vec2(pos.x - size.y * margin + offset, pos.y + size.y * margin);	//lf up
		t3 = vec2(tpos0.x, tpos1.y);

		LETTER character;
		character.p0 = p0;
		character.p1 = p1;
		character.p2 = p2;
		character.p3 = p3;
		character.t0 = t0;
		character.t1 = t1;
		character.t2 = t2;
		character.t3 = t3;
		rtext.string.push_back(character);
	}
	rtext.render = false;
	rtext.color = vec3(1.0, 1.0, 1.0);
	gui_string[name] = rtext;
}

void IGUI::deleteFromRenderQueue(string name) {
	if (data.count(name)) {
		data[name].render = false;
	//	getLogger()->LogDebug("GUI", "Disabled GUI Object: " + name);
		if (gui_string.count(name)) {
			gui_string[name].render = false;
	//		getLogger()->LogDebug("GUI", "Disabled String Object: " + name);
		}
		else {
			getLogger()->LogError("GUI", "Unknown String Object: " + name);
		}
	}
	else {
		getLogger()->LogError("GUI", "Unknown GUI Object: " + name);
	}
	
}

void IGUI::addToRenderQueue(string name) {
	if (data.count(name)) {
		data[name].render = true;
	//	getLogger()->LogDebug("GUI", "Enabled GUI Object: " + name);
		if (gui_string.count(name)) {
			gui_string[name].render = true;
	//		getLogger()->LogDebug("GUI", "Enabled String Object: " + name);
		}
		else {
			if (data[name].text.size() != 0) {
				getLogger()->LogError("GUI", "Unknown String Object: " + name);
			}
			
		}
	}
	else {
		getLogger()->LogError("GUI", "Unknown GUI Object: " + name);
	}
	
}

void IGUI::deleteFromRenderQueue(vector<string> name_) {
	for (const auto& name : name_) {
		if (data.count(name)) {
			data[name].render = false;
		//	getLogger()->LogDebug("GUI", "Disabled GUI Object: " + name);
			if (gui_string.count(name)) {
				gui_string[name].render = false;
		//		getLogger()->LogDebug("GUI", "Disabled String Object: " + name);
			}
			else {
				if (data[name].text.size() != 0) {
					getLogger()->LogError("GUI", "Unknown String Object: " + name);
				}
				
			}
		}
		else {
			getLogger()->LogError("GUI", "Unknown GUI Object: " + name);
		}
	}
	
}

void IGUI::addToRenderQueue(vector<string> name_) {
	for (const auto& name : name_) {
		if (data.count(name)) {
			data[name].render = true;
		//	getLogger()->LogDebug("GUI", "Enabled GUI Object: " + name);
			if (gui_string.count(name)) {
		//		getLogger()->LogDebug("GUI", "Enabled String Object: " + name);
				gui_string[name].render = true;
			}
			else {
				getLogger()->LogError("GUI", "Unknown String Object: " + name);
			}
		}
		else {
			getLogger()->LogError("GUI", "Unknown GUI Object: " + name);
		}
	}
	
}

ivec4 IGUI::getCharTextCoords(const char letter) {
	if (letter == 'a')
		return getPixelCoordsOffset(vec2(128, 128), vec2(8, 48), vec2(5, 8));
	if (letter == 'b')
		return getPixelCoordsOffset(vec2(128, 128), vec2(16, 48), vec2(5, 8));
	if (letter == 'c')
		return getPixelCoordsOffset(vec2(128, 128), vec2(24, 48), vec2(5, 8));
	if (letter == 'd')
		return getPixelCoordsOffset(vec2(128, 128), vec2(32, 48), vec2(5, 8));
	if (letter == 'e')
		return getPixelCoordsOffset(vec2(128, 128), vec2(40, 48), vec2(5, 8));
	if (letter == 'f')
		return getPixelCoordsOffset(vec2(128, 128), vec2(48, 48), vec2(5, 8));
	if (letter == 'g')
		return getPixelCoordsOffset(vec2(128, 128), vec2(56, 48), vec2(5, 8));
	if (letter == 'h')
		return getPixelCoordsOffset(vec2(128, 128), vec2(64, 48), vec2(5, 8));
	if (letter == 'i')
		return getPixelCoordsOffset(vec2(128, 128), vec2(72, 48), vec2(5, 8));
	if (letter == 'j')
		return getPixelCoordsOffset(vec2(128, 128), vec2(80, 48), vec2(5, 8));
	if (letter == 'k')
		return getPixelCoordsOffset(vec2(128, 128), vec2(88, 48), vec2(5, 8));
	if (letter == 'l')
		return getPixelCoordsOffset(vec2(128, 128), vec2(96, 48), vec2(5, 8));
	if (letter == 'm')
		return getPixelCoordsOffset(vec2(128, 128), vec2(104, 48), vec2(5, 8));
	if (letter == 'n')
		return getPixelCoordsOffset(vec2(128, 128), vec2(112, 48), vec2(5, 8));
	if (letter == 'o')
		return getPixelCoordsOffset(vec2(128, 128), vec2(120, 48), vec2(5, 8));
	if (letter == 'p')
		return getPixelCoordsOffset(vec2(128, 128), vec2(0, 56), vec2(5, 8));
	if (letter == 'q')
		return getPixelCoordsOffset(vec2(128, 128), vec2(8, 56), vec2(5, 8));
	if (letter == 'r')
		return getPixelCoordsOffset(vec2(128, 128), vec2(16, 56), vec2(5, 8));
	if (letter == 's')
		return getPixelCoordsOffset(vec2(128, 128), vec2(24, 56), vec2(5, 8));
	if (letter == 't')
		return getPixelCoordsOffset(vec2(128, 128), vec2(32, 56), vec2(5, 8));
	if (letter == 'u')
		return getPixelCoordsOffset(vec2(128, 128), vec2(40, 56), vec2(5, 8));
	if (letter == 'v')
		return getPixelCoordsOffset(vec2(128, 128), vec2(48, 56), vec2(5, 8));
	if (letter == 'w')
		return getPixelCoordsOffset(vec2(128, 128), vec2(56, 56), vec2(5, 8));
	if (letter == 'x')
		return getPixelCoordsOffset(vec2(128, 128), vec2(64, 56), vec2(5, 8));
	if (letter == 'y')
		return getPixelCoordsOffset(vec2(128, 128), vec2(72, 56), vec2(5, 8));
	if (letter == 'z')
		return getPixelCoordsOffset(vec2(128, 128), vec2(72, 56), vec2(5, 8));
	if (letter == 'A')
		return getPixelCoordsOffset(vec2(128, 128), vec2(8, 32), vec2(5, 8));
	if (letter == 'B')
		return getPixelCoordsOffset(vec2(128, 128), vec2(16, 32), vec2(5, 8));
	if (letter == 'C')
		return getPixelCoordsOffset(vec2(128, 128), vec2(24, 32), vec2(5, 8));
	if (letter == 'D')
		return getPixelCoordsOffset(vec2(128, 128), vec2(32, 32), vec2(5, 8));
	if (letter == 'E')
		return getPixelCoordsOffset(vec2(128, 128), vec2(40, 32), vec2(5, 8));
	if (letter == 'F')
		return getPixelCoordsOffset(vec2(128, 128), vec2(48, 32), vec2(5, 7));
	if (letter == 'G')
		return getPixelCoordsOffset(vec2(128, 128), vec2(56, 32), vec2(5, 8));
	if (letter == 'H')
		return getPixelCoordsOffset(vec2(128, 128), vec2(64, 32), vec2(5, 8));
	if (letter == 'I')
		return getPixelCoordsOffset(vec2(128, 128), vec2(72, 32), vec2(5, 8));
	if (letter == 'J')
		return getPixelCoordsOffset(vec2(128, 128), vec2(80, 32), vec2(5, 8));
	if (letter == 'K')
		return getPixelCoordsOffset(vec2(128, 128), vec2(88, 32), vec2(5, 8));
	if (letter == 'L')
		return getPixelCoordsOffset(vec2(128, 128), vec2(96, 32), vec2(5, 8));
	if (letter == 'M')
		return getPixelCoordsOffset(vec2(128, 128), vec2(104, 32), vec2(5, 8));
	if (letter == 'N')
		return getPixelCoordsOffset(vec2(128, 128), vec2(112, 32), vec2(5, 8));
	if (letter == 'O')
		return getPixelCoordsOffset(vec2(128, 128), vec2(120, 32), vec2(5, 8));
	if (letter == 'P')
		return getPixelCoordsOffset(vec2(128, 128), vec2(0, 40), vec2(5, 7));
	if (letter == 'Q')
		return getPixelCoordsOffset(vec2(128, 128), vec2(8, 40), vec2(5, 8));
	if (letter == 'R')
		return getPixelCoordsOffset(vec2(128, 128), vec2(16, 40), vec2(5, 8));
	if (letter == 'S')
		return getPixelCoordsOffset(vec2(128, 128), vec2(24, 40), vec2(5, 7));
	if (letter == 'T')
		return getPixelCoordsOffset(vec2(128, 128), vec2(32, 40), vec2(5, 8));
	if (letter == 'U')
		return getPixelCoordsOffset(vec2(128, 128), vec2(40, 40), vec2(5, 8));
	if (letter == 'V')
		return getPixelCoordsOffset(vec2(128, 128), vec2(48, 40), vec2(5, 8));
	if (letter == 'W')
		return getPixelCoordsOffset(vec2(128, 128), vec2(56, 40), vec2(5, 8));
	if (letter == 'X')
		return getPixelCoordsOffset(vec2(128, 128), vec2(64, 40), vec2(5, 8));
	if (letter == 'Y')
		return getPixelCoordsOffset(vec2(128, 128), vec2(72, 40), vec2(5, 8));
	if (letter == 'Z')
		return getPixelCoordsOffset(vec2(128, 128), vec2(80, 40), vec2(5, 8));

	if (letter == '0')
		return getPixelCoordsOffset(vec2(128, 128), vec2(0, 24), vec2(5, 7));
	if (letter == '1')
		return getPixelCoordsOffset(vec2(128, 128), vec2(8, 24), vec2(5, 7));
	if (letter == '2')
		return getPixelCoordsOffset(vec2(128, 128), vec2(16, 24), vec2(5, 7));
	if (letter == '3')
		return getPixelCoordsOffset(vec2(128, 128), vec2(24, 24), vec2(5, 7));
	if (letter == '4')
		return getPixelCoordsOffset(vec2(128, 128), vec2(32, 24), vec2(5, 7));
	if (letter == '5')
		return getPixelCoordsOffset(vec2(128, 128), vec2(40, 24), vec2(5, 7));
	if (letter == '6')
		return getPixelCoordsOffset(vec2(128, 128), vec2(48, 24), vec2(5, 7));
	if (letter == '7')
		return getPixelCoordsOffset(vec2(128, 128), vec2(56, 24), vec2(5, 7));
	if (letter == '8')
		return getPixelCoordsOffset(vec2(128, 128), vec2(64, 24), vec2(5, 7));
	if (letter == '9')
		return getPixelCoordsOffset(vec2(128, 128), vec2(72, 24), vec2(5, 7));

	if (letter == '-')
		return getPixelCoordsOffset(vec2(128, 128), vec2(104,16), vec2(5, 7));
	if (letter == '.')
		return getPixelCoordsOffset(vec2(128, 128), vec2(110,16), vec2(5, 7));

	return vec4();


}

ivec4 IGUI::getPixelCoords(ivec2 TextureSize, ivec2 lu, ivec2 rd) {
	return vec4(lu.x / TextureSize.x, lu.y / TextureSize.y, rd.x / TextureSize.x, rd.y / TextureSize.y);
}

ivec4 IGUI::getPixelCoordsOffset(ivec2 TextureSize, ivec2 lu, ivec2 offset) {
	return vec4(lu.x / TextureSize.x, lu.y / TextureSize.y, (lu.x + offset.x) / TextureSize.x, (lu.y + offset.y) / TextureSize.y);
}

void IGUI::prepareRenderer() {
	vector<GLuint> indices[16]{};
	vector<GLfloat> renderingData[16]{};

	vector<GLuint> indicesStr;
	vector<GLfloat> renderingDataStr;

	for (const auto& guiData : data) {
		GUI_Data gui = guiData.second;
		if (gui.render) {
			renderingData[gui.renderstep].push_back(gui.p0.x);
			renderingData[gui.renderstep].push_back(gui.p0.y);
			renderingData[gui.renderstep].push_back(gui.color.r);
			renderingData[gui.renderstep].push_back(gui.color.g);
			renderingData[gui.renderstep].push_back(gui.color.b);
			renderingData[gui.renderstep].push_back(gui.t0.x);
			renderingData[gui.renderstep].push_back(gui.t0.y);
			renderingData[gui.renderstep].push_back(gui.p1.x);
			renderingData[gui.renderstep].push_back(gui.p1.y);
			renderingData[gui.renderstep].push_back(gui.color.r);
			renderingData[gui.renderstep].push_back(gui.color.g);
			renderingData[gui.renderstep].push_back(gui.color.b);
			renderingData[gui.renderstep].push_back(gui.t1.x);
			renderingData[gui.renderstep].push_back(gui.t1.y);
			renderingData[gui.renderstep].push_back(gui.p2.x);
			renderingData[gui.renderstep].push_back(gui.p2.y);
			renderingData[gui.renderstep].push_back(gui.color.r);
			renderingData[gui.renderstep].push_back(gui.color.g);
			renderingData[gui.renderstep].push_back(gui.color.b);
			renderingData[gui.renderstep].push_back(gui.t2.x);
			renderingData[gui.renderstep].push_back(gui.t2.y);
			renderingData[gui.renderstep].push_back(gui.p3.x);
			renderingData[gui.renderstep].push_back(gui.p3.y);
			renderingData[gui.renderstep].push_back(gui.color.r);
			renderingData[gui.renderstep].push_back(gui.color.g);
			renderingData[gui.renderstep].push_back(gui.color.b);
			renderingData[gui.renderstep].push_back(gui.t3.x);
			renderingData[gui.renderstep].push_back(gui.t3.y);
		}
	}

	for (const auto& strData : gui_string) {
		string_rendering_data stringData = strData.second;

		float shadowOffset = .20;

		if (stringData.render) {
			for (const auto& letter : stringData.string) {
				renderingDataStr.push_back(letter.p0.x + stringData.letterSize * shadowOffset);
				renderingDataStr.push_back(letter.p0.y - stringData.letterSize * shadowOffset);
				renderingDataStr.push_back(stringData.color.r / 4.0f);
				renderingDataStr.push_back(stringData.color.g / 4.0f);
				renderingDataStr.push_back(stringData.color.b / 4.0f);
				renderingDataStr.push_back(letter.t0.x);
				renderingDataStr.push_back(letter.t0.y);
				renderingDataStr.push_back(letter.p1.x + stringData.letterSize * shadowOffset);
				renderingDataStr.push_back(letter.p1.y - stringData.letterSize * shadowOffset);
				renderingDataStr.push_back(stringData.color.r / 4.0f);
				renderingDataStr.push_back(stringData.color.g / 4.0f);
				renderingDataStr.push_back(stringData.color.b / 4.0f);
				renderingDataStr.push_back(letter.t1.x);
				renderingDataStr.push_back(letter.t1.y);
				renderingDataStr.push_back(letter.p2.x + stringData.letterSize * shadowOffset);
				renderingDataStr.push_back(letter.p2.y - stringData.letterSize * shadowOffset);
				renderingDataStr.push_back(stringData.color.r / 4.0f);
				renderingDataStr.push_back(stringData.color.g / 4.0f);
				renderingDataStr.push_back(stringData.color.b / 4.0f);
				renderingDataStr.push_back(letter.t2.x);
				renderingDataStr.push_back(letter.t2.y);
				renderingDataStr.push_back(letter.p3.x + stringData.letterSize * shadowOffset);
				renderingDataStr.push_back(letter.p3.y - stringData.letterSize * shadowOffset);
				renderingDataStr.push_back(stringData.color.r / 4.0f);
				renderingDataStr.push_back(stringData.color.g / 4.0f);
				renderingDataStr.push_back(stringData.color.b / 4.0f);
				renderingDataStr.push_back(letter.t3.x);
				renderingDataStr.push_back(letter.t3.y);


				renderingDataStr.push_back(letter.p0.x);
				renderingDataStr.push_back(letter.p0.y);
				renderingDataStr.push_back(stringData.color.r);
				renderingDataStr.push_back(stringData.color.g);
				renderingDataStr.push_back(stringData.color.b);
				renderingDataStr.push_back(letter.t0.x);
				renderingDataStr.push_back(letter.t0.y);
				renderingDataStr.push_back(letter.p1.x);
				renderingDataStr.push_back(letter.p1.y);
				renderingDataStr.push_back(stringData.color.r);
				renderingDataStr.push_back(stringData.color.g);
				renderingDataStr.push_back(stringData.color.b);
				renderingDataStr.push_back(letter.t1.x);
				renderingDataStr.push_back(letter.t1.y);
				renderingDataStr.push_back(letter.p2.x);
				renderingDataStr.push_back(letter.p2.y);
				renderingDataStr.push_back(stringData.color.r);
				renderingDataStr.push_back(stringData.color.g);
				renderingDataStr.push_back(stringData.color.b);
				renderingDataStr.push_back(letter.t2.x);
				renderingDataStr.push_back(letter.t2.y);
				renderingDataStr.push_back(letter.p3.x);
				renderingDataStr.push_back(letter.p3.y);
				renderingDataStr.push_back(stringData.color.r);
				renderingDataStr.push_back(stringData.color.g);
				renderingDataStr.push_back(stringData.color.b);
				renderingDataStr.push_back(letter.t3.x);
				renderingDataStr.push_back(letter.t3.y);

			}
			
		}
	}

	for (int i0 = 0; i0 < 16; i0++) {
		if (renderingData[i0].size() != 0) {
			for (int i = 0; i <= ((int)((renderingData[i0].size() - 1) / 28)); i++) {
				indices[i0].insert(indices[i0].end(), { (i * 4u), 1 + (i * 4u),3 + (i * 4u) ,1 + (i * 4u) ,2 + (i * 4u) ,3 + (i * 4u) });
			}
		}
	}

	if (renderingDataStr.size() != 0) {
		for (int i = 0; i <= ((int)((renderingDataStr.size() - 1) / 28)); i++) {
			indicesStr.insert(indicesStr.end(), { (i * 4u), 1 + (i * 4u),3 + (i * 4u) ,1 + (i * 4u) ,2 + (i * 4u) ,3 + (i * 4u) });
		}
	}

	for (int i = 0; i < 16; i++) {
//		if (renderingData[i].size() != 0) {
			glBindVertexArray(VAO[i]);

			glBindBuffer(GL_ARRAY_BUFFER, VBO[i]);
			glBufferData(GL_ARRAY_BUFFER, renderingData[i].size() * sizeof(float), renderingData[i].data(), GL_STATIC_DRAW);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[i]);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices[i].size() * sizeof(unsigned int), indices[i].data(), GL_STATIC_DRAW);

			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(0 * sizeof(GL_FLOAT)));
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(2 * sizeof(GL_FLOAT)));
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(5 * sizeof(GL_FLOAT)));
			glEnableVertexAttribArray(2);
			glBindVertexArray(0);
			size[i] = indices[i].size();
//		}

	}

//	if (renderingDataStr.size() != 0) {
		glBindVertexArray(fontVAO);

		glBindBuffer(GL_ARRAY_BUFFER, fontVBO);
		glBufferData(GL_ARRAY_BUFFER, renderingDataStr.size() * sizeof(float), renderingDataStr.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, fontEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesStr.size() * sizeof(unsigned int), indicesStr.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(0 * sizeof(GL_FLOAT)));
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(2 * sizeof(GL_FLOAT)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(5 * sizeof(GL_FLOAT)));
		glEnableVertexAttribArray(2);
		glBindVertexArray(0);
		fontsize = indicesStr.size();
}
	

void IGUI::Render() {
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	for (int i = 0; i < 16; i++) {
		if (size[i] != 0) {
			glBindTexture(GL_TEXTURE_2D, texture[i].get());
			//std::cout << texture[i].get() << "\n";
			guiShader[i].use();
			glBindVertexArray(VAO[i]);
			glDrawElements(GL_TRIANGLES, size[i], GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
		}
	}
	if (fontsize != 0) {
		glBindTexture(GL_TEXTURE_2D, font.get());
		guiShader[15].use();
		glBindVertexArray(fontVAO);
		glDrawElements(GL_TRIANGLES, fontsize, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}
	glDisable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
}

GUI_Data* IGUI::getGUI(string name) {
	if (data.count(name)) {
		return &data[name];
	}
	else {
		getLogger()->LogError("GUI", "Cannot find GUI Object " + name);
	}
	return nullptr;
}

void IGUI::changeGUIcall(std::string name, std::vector<std::string> add, std::vector<std::string> del) {
	if (data.count(name)) {
		data[name].guiAdd = add;
		data[name].guiDel = del;
	}
	else {
	//	if (debug0 == DEBUG_COLLUSION || debug1 == DEBUG_COLLUSION || debug2 == DEBUG_COLLUSION || debug3 == DEBUG_COLLUSION) {
		getLogger()->LogError("GUI", "Cannot find GUI Object while changing GUI Call: " + name);
		//}
	}
}

void IGUI::changeGUIEvent(std::string name, int EVENT) {
	if (data.count(name)) {
		data[name].gevent = EVENT;
	}
	else {
		//	if (debug0 == DEBUG_COLLUSION || debug1 == DEBUG_COLLUSION || debug2 == DEBUG_COLLUSION || debug3 == DEBUG_COLLUSION) {
		getLogger()->LogError("GUI", "Cannot find GUI Object while changing GUI Events: " + name);
		//}
	}
}

void IGUI::clickDetection() {
	if (clicked) {
		int sx, sy;

		glfwGetWindowSize(window, &sx, &sy);

		float x = (MouseLeftButtonPressPos.x / (float)sx) * 2 - 1;
		float y = (MouseLeftButtonPressPos.y / (float)sy) * 2 - 1;

		float aspectx = (float)sx / (float)sy;

		y = -y;

		x = x * aspectx;

		for (const auto& data : data) {
			GUI_Data gui = data.second;
			if (gui.render) {
				
				if (gui.clickable) {
					string clicked = "No";
					if ((gui.p0.x <= x && gui.p0.y <= y
						&& gui.p1.x >= x && gui.p1.y <= y
						&& gui.p2.x >= x && gui.p2.y >= y
						&& gui.p3.x <= x && gui.p3.y >= y)) {
						clicked = "Yes";
						tickable.push_back(gui.name);

					}
				}
			}
		}
		clicked = false;
	}
	
}

void IGUI::tick() {

	bool update = false;

	for (const auto& gui_ : tickable) {
		GUI_Data gui = data[gui_];
		if ((int)gui.guiDel.size() != 0) {
			deleteFromRenderQueue(gui.guiDel);
			update = true;
		}
		if ((int)gui.guiAdd.size() != 0) {
			addToRenderQueue(gui.guiAdd);
			update = true;
		}
		if (gui.gevent != NULL) {
			CLIENT_EVENTS->push_back(gui.gevent);
		}
	}
	if (update) {
		prepareRenderer();
	}

	tickable.clear();
}

void IGUI::update() {

	int x, y;
	glfwGetWindowSize(window,&x, &y);

	float aspectx = (float)x / (float)y;

	mat4 projection = ortho(aspectx * -1.f, aspectx * 1.f, -1.f, 1.f, -1.f, 1.f);

	for (int i = 0; i < 16; i++) {
		guiShader[i].setMat4("proj", projection);
	}
	

	clickDetection();
	tick();
	
}

void IGUI::configTexture(int renderStep, std::string tdir, int index) {
	texture[renderStep - 1].Load(tdir.c_str());
	guiShader[renderStep - 1].bindTexture2D(index - 1, texture[renderStep - 1].get(), "texture0");
}

void IGUI::configTexture(int renderStep, TexturedFrameBuffer fbo, int index) {
	getLogger()->LogDebug("GUI", "Changed Texture ID: " + std::to_string(fbo.texture));
	guiShader[renderStep - 1].bindTexture2D(index - 1, fbo.texture, "texture0");
}


void IGUI::init(GLFWwindow* window_, vector<int>* CE) {
	
	CLIENT_EVENTS = CE;
	
	
	for (int i = 0; i < 16; i++) {
		glGenVertexArrays(1, &VAO[i]);
		glGenBuffers(1, &EBO[i]);
		glGenBuffers(1, &VBO[i]);
		guiShader[i].init("assets/shaders/GUI/vert.glsl", "assets/shaders/GUI/frag.glsl");
		mat4 projection = ortho(-1.f, 1.f, -1.f, 1.f, -1.f, 1.f);

		guiShader[i].setMat4("proj", projection);
		guiShader[i].setVec3("clr", 1.f, 1.0f, 1.f);
		getLogger()->LogDebug("GUI" , "Initalized Shader: " + std::to_string(guiShader[i].ShaderID));
	}
	glGenVertexArrays(1, &fontVAO);
	glGenBuffers(1, &fontEBO);
	glGenBuffers(1, &fontVBO);
	font.Load("assets/fonts/default.png");
	

	window = window_;
}