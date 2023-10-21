#pragma once
#include "../Texture.h"
#include <vector>
#include "../ImageLoader.h"
#include <iostream>
#include <fstream>
class TextureArray : public Texture {
public:
	void UploadToGPU() {


		std::ofstream file;
		file.open("BlockTexDump.dmp");
		for (size_t i = 0; i < ArrayData.size(); i++) {
			file << ArrayData.data()[i];
		}
		file.close();

		GLsizei mipLevelCount = 3;

		glBindTexture(GL_TEXTURE_2D_ARRAY, textureID);
		glTexStorage3D(GL_TEXTURE_2D_ARRAY, mipLevelCount, GL_RGBA8, 16, 16, Layers);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0, width, height, Layers, GL_RGBA, GL_UNSIGNED_BYTE, ArrayData.data());
		glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
	}
	void SetSize(int width_, int height_) {
		width = width_;
		height = height_;
	}
	bool AddTextureToArray(RawTextureData* Data) {
		Format = GL_RGBA;
		if (Data->data) {
			if (width == Data->width && height == Data->height) {
				if (Data->format == GL_RGB) {
					for (int index = 0; index < Data->width * Data->height; index++) {
						ArrayData.push_back(Data->data[(index * 3)]);
						ArrayData.push_back(Data->data[(index * 3) + 1]);
						ArrayData.push_back(Data->data[(index * 3) + 2]);
						ArrayData.push_back(255);
					}
					Layers++;
				}
				if (Data->format == GL_RGBA) {
					for (int index = 0; index < Data->width * Data->height; index++) {
						ArrayData.push_back(Data->data[(index * 4)]);
						ArrayData.push_back(Data->data[(index * 4) + 1]);
						ArrayData.push_back(Data->data[(index * 4) + 2]);
						ArrayData.push_back(Data->data[(index * 4) + 3]);
					}
					Layers++;
				}
			}
			else {
				Logger.LogError("Texture Array Loader", "Width or height doesn't match");
				return false;
			}


		}
		else {
			Logger.LogError("Texture Array Loader", "No texture");
			return false;
		}
		return true;
	}
	bool AddTextureToArray(std::string file) {
		RawTextureData tex = GetImageData(file.c_str());
		if (AddTextureToArray(&tex)) {
			Logger.LogInfo("Image Loader", "Loaded: " + file + " | Size: " + std::to_string(tex.height) + ", " + std::to_string(tex.width));
			return true;
		}
		Logger.LogError("Image Loader", "Unable to load: " + file);
		return false;
	}
	int GetLayers() {
		return Layers;
	}

	std::vector<unsigned char> ArrayData;

private:

	int Layers = 0;

};