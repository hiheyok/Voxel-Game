#include "Font.h"

using namespace glm;
using namespace std;

vec4 RenderableFont::GetCharUV(char c) {
	int index = (int)c;

	float Width = GetCharWidth(c);

	float x0 = (float)(index & 0b1111);
	float y0 = (float)(index >> 4);

	float x1 = x0 + Width;
	float y1 = y0 + 1.f;

	x0 = x0 / 16.f;
	y0 = y0 / 16.f;
	x1 = x1 / 16.f;
	y1 = y1 / 16.f;

	return vec4(x0, 1.f - y0, x1, 1.f - y1);
}

float RenderableFont::GetCharWidth(char c) {
	return CharWidth[(int)c];
}


vector<float> RenderableFont::GetCharactorVertex(char c, float xOffset, float yOffset, float xOrigin, float yOrigin) {
	

	vec4 UV = GetCharUV(c);

	float x0 = UV.x;
	float y0 = UV.y;
	float x1 = UV.z;
	float y1 = UV.w;

	float Width = GetCharWidth(c);

	vector<float> out = {
		0, 0 ,xOffset, yOffset, xOrigin, yOrigin, x0, y1, color.x, color.y, color.z,
		0 + fontSize * Width, 0 ,xOffset, yOffset, xOrigin, yOrigin, x1, y1, color.x, color.y, color.z,
		0 + fontSize * Width, 0 + fontSize ,xOffset, yOffset, xOrigin, yOrigin, x1, y0, color.x, color.y, color.z,
		0 + fontSize * Width, 0 + fontSize ,xOffset, yOffset, xOrigin, yOrigin, x1, y0, color.x, color.y, color.z,
		0 , 0 + fontSize ,xOffset, yOffset, xOrigin, yOrigin, x0, y0, color.x, color.y, color.z,
		0 , 0 ,xOffset, yOffset, xOrigin, yOrigin, x0, y1, color.x, color.y, color.z
	};

	return out;
}

vector<float> RenderableFont::getVertices() {
	vector<float> out;

	float Shift = fontSize;

	vec2 coords = Position;

	float OffsetValue = 0.f;

	for (int i = 0; i < RenderText.size(); i++) {
		char c = RenderText[i];
		float Width = GetCharWidth(c);
		float ShiftMag = Shift * Width + spacing * Shift;

		vector<float> CharVert = GetCharactorVertex(RenderText[i], OffsetValue, 0.f, coords.x, coords.y);

		OffsetValue = OffsetValue + ShiftMag;

		out.insert(out.end(), CharVert.begin(), CharVert.end());
	}

	return out;
}