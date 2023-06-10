#pragma once
#include "../Texture.h"
#include <vector>

class TextureArray : public Texture {
public:
	void Load(RawTextureData data) {
		if (data.data) {
			glBindTexture(GL_TEXTURE_2D, textureID);
			glTexImage2D(GL_TEXTURE_2D, 0, data.format, data.width, data.height, 0, data.format, GL_UNSIGNED_BYTE, data.data);
			glGenerateMipmap(GL_TEXTURE_2D);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

			GLsizei mipLevelCount = 5;

			glBindTexture(GL_TEXTURE_2D_ARRAY, textureID);
			glTexStorage3D(GL_TEXTURE_2D_ARRAY, mipLevelCount, GL_RGBA8, 16, 16, Layers);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0, width, height, Layers, GL_RGBA, GL_UNSIGNED_BYTE, ArrayData.data());
			glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
		}
		else {
			getLogger()->LogError("Texture Loader", "Image is not loaded");
		}

	}
	void SetSize(int width_, int height_) {
		width = width_;
		height = height_;
	}
	bool AddTextureToArray(RawTextureData* Data) {

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
				getLogger()->LogError("Texture Array Loader", "Width or height doesn't match");
				return false;
			}
			
			
		}else {
			getLogger()->LogError("Texture Array Loader","No texture");
			return false;
		}
		return true;
	}

	int GetLayers() {
		return Layers;
	}

	int width = NULL;
	int height = NULL;

private:
	std::vector<unsigned int> ArrayData;
	int Layers = 0;
	
};