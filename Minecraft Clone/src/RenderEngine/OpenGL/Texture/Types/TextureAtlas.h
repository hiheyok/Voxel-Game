#pragma once
#include "../Texture.h"
#include <vector>
#include "../ImageLoader.h"
#include <iostream>
#include <fstream>
#include <glm/vec4.hpp>
#include <optional>
class TextureAtlas : public Texture {
public:
	void UploadToGPU() {
		GLsizei mipLevelCount = 4;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, TextureAtlasData.data());
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 4);

		Logger.LogDebug("Texture Loader", "Loaded 2D Texture: " + std::to_string(textureID));
	}
	void SetSize(int width_, int height_) {
		width = width_;
		height = height_;

		TextureAtlasData.resize(width * height * colorSpace, NULL);
	}

	inline void setPixel(uint8_t r, uint8_t  g, uint8_t b, uint8_t a, int w, int h) {
		TextureAtlasData[w * colorSpace + h * colorSpace + 0] = r;
		TextureAtlasData[w * colorSpace + h * colorSpace + 1] = g;
		TextureAtlasData[w * colorSpace + h * colorSpace + 2] = b;
		TextureAtlasData[w * colorSpace + h * colorSpace + 3] = a;
	}

	void AddData(std::vector<uint8_t> Data, int Format, bool& transparency) { //assumes that all textures  are 16 x 16
		//Get offset to where to put the texture
		int WidthTex = width / 16;
		int heightTex = height / 16;

		int WidthIndex = Count % WidthTex;
		int HeightIndex = Count / WidthTex;

		int WidthIndexPixel = WidthIndex * 16; //Offset for the width
		int HeightIndexPixel = width * HeightIndex * 16; //Offset for the height

		if (Format == GL_RGB) {
			for (int index = 0; index < 16 * 16; index++) {
				setPixel(Data[(index * 3)], Data[(index * 3) + 1], Data[(index * 3) + 2], 255, WidthIndexPixel + (index % 16), HeightIndexPixel + width * (index /  16));
			}
			Count++;
		}
		else  if (Format == GL_RG) {
			for (int index = 0; index < 16 * 16; index++) {
				if (Data[(index * 2) + 1] != 255) {
					transparency = true;
				}

				setPixel(Data[(index * 2)], Data[(index * 2)], Data[(index * 2)], Data[(index * 2) + 1], WidthIndexPixel + (index % 16), HeightIndexPixel + width * (index / 16));
			}
			Count++;
		}
		else  if (Format == GL_RGBA) {
			for (int index = 0; index < 16 * 16; index++) {
				if (Data[(index * 4) + 3] != 255) {
					transparency = true;
				}
				setPixel(Data[(index * 4)], Data[(index * 4) + 1], Data[(index * 4) + 2], Data[(index * 4) + 3], WidthIndexPixel + (index % 16), HeightIndexPixel + width * (index / 16));
			}
			Count++;
		}
		else if (Format == GL_RED) {
			for (int index = 0; index < 16 * 16; index++) {
				setPixel(Data[index], Data[index], Data[index], 255, WidthIndexPixel + (index % 16), HeightIndexPixel + width * (index / 16));
			}
			Count++;
		}
		else {
			Logger.LogError("Texture Atlas Stitcher", "Invalid image format!");
		}
	}

	bool _AddTextureToAtlas(RawTextureData* Data, bool& transparency) {

		Format = GL_RGBA;
		if (!Data->data) {
			Logger.LogError("Texture Atlas", "No texture");
			return false;
		}

		if (((Data->width % 16) != 0) || ((Data->height % 16) != 0)) {
			Logger.LogError("Texture Atlas", "Width or height doesn't match");
			return false;
		}

		int ImgsX = Data->width / 16;
		int ImgsY = Data->height / 16;

		int ColorSize = 3;

		if (Data->format == GL_RGBA) {
			ColorSize = 4;
		}
		if (Data->format ==  GL_RG) {
			ColorSize = 2;
		}
		else if (Data->format == GL_RED) {
			ColorSize = 1;
		}

		int CWidth = 16 * ColorSize;

		for (int x = 0; x < ImgsX; x++) {
			for (int y = 0; y < ImgsY; y++) {
				int gx = x * 16 * ColorSize;
				int gy = y * 16 * Data->width * ColorSize;

				std::vector<uint8_t> buffer(16 * 16 * ColorSize);

				for (int h = 0; h < 16; h++) {
					memcpy(buffer.data() + (h * CWidth), Data->data + (h * Data->width * ColorSize + gx + gy), CWidth);
				}

				AddData(buffer, Data->format, transparency);
			}
		}

		return true;
	}

	std::optional<RawTextureData> AddTextureToAtlas(std::string file, bool& transparency) {
		std::optional<RawTextureData> data;
		RawTextureData tex = GetImageData(file.c_str());
		if (_AddTextureToAtlas(&tex, transparency)) {
			Logger.LogInfo("Texture Atlas", "Loaded: " + file + " | Size: " + std::to_string(tex.height) + ", " + std::to_string(tex.width));
			data = tex;
			return data;
		}
		Logger.LogError("Texture Atlas", "Unable to load: " + file);
		return data;
	}

	int GetBlockCount() {
		return Count;
	}

	std::vector<unsigned char> TextureAtlasData;
	const int colorSpace = 4;
	bool isTransparent = false;
	int Count = 0;

};