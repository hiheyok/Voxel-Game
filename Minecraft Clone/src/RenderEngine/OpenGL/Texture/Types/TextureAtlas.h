#pragma once
#include "../Texture.h"
#include <vector>
#include "../ImageLoader.h"
#include <iostream>
#include <fstream>
#include <glm/vec4.hpp>
#include <optional>
#include <cctype>
class TextureAtlas : public Texture {
public:
	void UploadToGPU() {
		// GLsizei mipLevelcount_ = 4;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width_, height_, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture_atlas_data_.data());
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 4);

		Logger.LogDebug("Texture Loader", "Loaded 2D Texture: " + std::to_string(textureID));
	}
	void SetSize(int width, int height) {
		width_ = width;
		height_ = height;

		texture_atlas_data_.resize(width_ * height_ * color_space_, NULL);
	}

	inline void setPixel(uint8_t r, uint8_t  g, uint8_t b, uint8_t a, size_t w, size_t h) {
		texture_atlas_data_[w * color_space_ + h * color_space_ + 0] = r;
		texture_atlas_data_[w * color_space_ + h * color_space_ + 1] = g;
		texture_atlas_data_[w * color_space_ + h * color_space_ + 2] = b;
		texture_atlas_data_[w * color_space_ + h * color_space_ + 3] = a;
	}

	void AddData(std::vector<uint8_t> Data, int Format, bool& transparency, bool& is_see_through_) { //assumes that all textures  are 16 x 16
		//Get offset to where to put the texture
		size_t WidthTex = width_ / 16;
		//int heightTex = height / 16;

		size_t WidthIndex = count_ % WidthTex;
		size_t HeightIndex = count_ / WidthTex;

		size_t WidthIndexPixel = WidthIndex * 16; //Offset for the width
		size_t HeightIndexPixel = width_ * HeightIndex * 16; //Offset for the height

		switch (Format) {
		case GL_RGB:
			for (int index = 0; index < 16 * 16; index++) {
				setPixel(Data[(index * 3)], Data[(index * 3) + 1], Data[(index * 3) + 2], 255, WidthIndexPixel + (index % 16), HeightIndexPixel + width_ * (index / 16));
			}
			count_++;
			break;
		case GL_RG:
			for (int index = 0; index < 16 * 16; index++) {
				if ((Data[(index * 2) + 1] != 255) && (Data[(index * 2) + 1] != 0)) {
					transparency = true;
					is_see_through_ = true;
				}

				if (Data[(index * 2) + 1] != 255) {
					is_see_through_ = true;
				}

				setPixel(Data[(index * 2)], Data[(index * 2)], Data[(index * 2)], Data[(index * 2) + 1], WidthIndexPixel + (index % 16), HeightIndexPixel + width_ * (index / 16));
			}
			count_++;
			break;
		case GL_RGBA:
			for (int index = 0; index < 16 * 16; index++) {
				if ((Data[(index * 4) + 3] != 255) && (Data[(index * 4) + 3] != 0)) {
					transparency = true;
					is_see_through_ = true;
				}

				if (Data[(index * 4) + 3] != 255) {
					is_see_through_ = true;
				}
				setPixel(Data[(index * 4)], Data[(index * 4) + 1], Data[(index * 4) + 2], Data[(index * 4) + 3], WidthIndexPixel + (index % 16), HeightIndexPixel + width_ * (index / 16));
			}
			count_++;
			break;
		case GL_RED:
			for (int index = 0; index < 16 * 16; index++) {
				setPixel(Data[index], Data[index], Data[index], 255, WidthIndexPixel + (index % 16), HeightIndexPixel + width_ * (index / 16));
			}
			count_++;
			break;
		default:
			Logger.LogError("Texture Atlas Stitcher", "Invalid image format!");
			break;
		}
	}

	bool _AddTextureToAtlas(RawTextureData* Data, bool& transparency, bool& is_see_through_) {

		format_ = GL_RGBA;
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

		int colorSize = 3;

		if (Data->format == GL_RGBA) {
			colorSize = 4;
		}
		if (Data->format ==  GL_RG) {
			colorSize = 2;
		}
		else if (Data->format == GL_RED) {
			colorSize = 1;
		}

		int cWidth = 16 * colorSize;

		for (int x = 0; x < ImgsX; x++) {
			for (int y = 0; y < ImgsY; y++) {
				int gx = x * 16 * colorSize;
				int gy = y * 16 * Data->width * colorSize;

				std::vector<uint8_t> buffer(16 * 16 * colorSize);

				for (int h = 0; h < 16; h++) {
					memcpy(buffer.data() + (h * cWidth), Data->data + (h * Data->width * colorSize + gx + gy), cWidth);
				}

				AddData(buffer, Data->format, transparency, is_see_through_);
			}
		}

		return true;
	}

	std::optional<RawTextureData> AddTextureToAtlas(std::string file, bool& transparency, bool& is_see_through_) {
		std::optional<RawTextureData> data;
		RawTextureData tex = GetImageData(file.c_str());
		if (_AddTextureToAtlas(&tex, transparency, is_see_through_)) {
			Logger.LogInfo("Texture Atlas", "Loaded: " + file + " | Size: " + std::to_string(tex.height) + ", " + std::to_string(tex.width));
			data = tex;
			return data;
		}
		Logger.LogError("Texture Atlas", "Unable to load: " + file);
		return data;
	}

	size_t GetBlockCount() {
		return count_;
	}

	std::vector<uint8_t> texture_atlas_data_;
	const int color_space_ = 4;
	bool is_transparent_ = false;
	size_t count_ = 0;

};