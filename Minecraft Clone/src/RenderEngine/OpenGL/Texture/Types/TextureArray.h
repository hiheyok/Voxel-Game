#pragma once
#include "../Texture.h"
#include <vector>
#include "../ImageLoader.h"
#include <iostream>
#include <fstream>
#include <optional>
class TextureArray : public Texture {
public:
	void UploadToGPU() {
		GLsizei mipLevelCount = 4;

		glBindTexture(GL_TEXTURE_2D_ARRAY, textureID);
		glTexStorage3D(GL_TEXTURE_2D_ARRAY, mipLevelCount, GL_RGBA8, 16, 16, Layers);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0, width_, height_, Layers, GL_RGBA, GL_UNSIGNED_BYTE, ArrayData.data());
		glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
		Logger.LogDebug("Texture Loader", "Loaded Texture Array: " + std::to_string(textureID));
	}
	void SetSize(int width, int height) {
		width_ = width;
		height_ = height;
	}

	void AddData(std::vector<uint8_t> Data, size_t Width, size_t Height, int Format) {
		if (Format == GL_RGB) {
			for (size_t index = 0; index < Width * Height; index++) {
				ArrayData.push_back(Data[(index * 3)]);
				ArrayData.push_back(Data[(index * 3) + 1]);
				ArrayData.push_back(Data[(index * 3) + 2]);
				ArrayData.push_back(255);
			}
			Layers++;
		} else  if (Format == GL_RGBA) {
			for (size_t index = 0; index < Width * Height; index++) {
				ArrayData.push_back(Data[(index * 4)]);
				ArrayData.push_back(Data[(index * 4) + 1]);
				ArrayData.push_back(Data[(index * 4) + 2]);
				ArrayData.push_back(Data[(index * 4) + 3]);
			}
			Layers++;
		}
		else  if (Format == GL_RG) {
			for (size_t index = 0; index < Width * Height; index++) {
				ArrayData.push_back(Data[(index * 2)]);
				ArrayData.push_back(Data[(index * 2)]);
				ArrayData.push_back(Data[(index * 2)]);
				ArrayData.push_back(Data[(index * 2) + 1]);
			}
			Layers++;
		}
		else if (Format == GL_RED) {
			for (size_t index = 0; index < Width * Height; index++) {
				ArrayData.push_back(Data[index]);
				ArrayData.push_back(Data[index]);
				ArrayData.push_back(Data[index]);
				ArrayData.push_back(255);
			}
			Layers++;
		}
		else {
			throw std::runtime_error("Invalid texture type not handled");
		}
	}

	bool _AddTextureToArray(RawTextureData* Data) {


		format_ = GL_RGBA;
		if (!Data->data) {
			Logger.LogError("Texture Array Loader", "No texture");
			return false;
		}

		if (((Data->width % width_) != 0) || ((Data->height % height_) != 0)) {
			Logger.LogError("Texture Array Loader", "Width or height doesn't match");
			return false;
		}

		size_t ImgsX = Data->width / width_;
		size_t ImgsY = Data->height / height_;

		int colorSize = 3;

		if (Data->format == GL_RGBA) {
			colorSize = 4;
		}
		if (Data->format == GL_RG) {
			colorSize = 2;
		}
		else if (Data->format == GL_RED) {
			colorSize = 1;
		}

		size_t cWidth = width_ * colorSize;

		for (int x = 0; x < ImgsX; x++) {
			for (int y = 0; y < ImgsY; y++) {
				size_t gx = x * width_ * colorSize;
				size_t gy = y * height_ * Data->width * colorSize;

				std::vector<uint8_t> buffer(width_ * height_ * colorSize);

				for (int h = 0; h < height_; h++) {
					memcpy(buffer.data() + (h * cWidth), Data->data + (h * Data->width * colorSize + gx + gy), cWidth);
				}

				AddData(buffer, width_, height_, Data->format);
			}
		}

		return true;
	}
	std::optional<RawTextureData> AddTextureToArray(std::string file) {
		std::optional<RawTextureData> data;
		RawTextureData tex = GetImageData(file.c_str());
		if (_AddTextureToArray(&tex)) {
			Logger.LogInfo("Image Loader", "Loaded: " + file + " | Size: " + std::to_string(tex.height) + ", " + std::to_string(tex.width));
			data = tex;
			return data;
		}
		Logger.LogError("Image Loader", "Unable to load: " + file);
		return data;
	}

	int GetLayers() {
		return Layers;
	}

	std::vector<unsigned char> ArrayData;

	int Layers = 0;

};