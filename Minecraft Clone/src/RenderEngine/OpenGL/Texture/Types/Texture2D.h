#pragma once
#include "../Texture.h"
#include "../../../../Utils/LogUtils.h"

class Texture2D : public Texture {
public:
    Texture2D(RawTextureData data) {
        Logger.LogDebug("GUI", "Addex TextureO*(&BNT*O(&");
        Load(data);
    }

    Texture2D() {
    }

	bool Load(RawTextureData data) override {
        if (data.data) {
            glBindTexture(GL_TEXTURE_2D, textureID);
            glTexImage2D(GL_TEXTURE_2D, 0, data.format, data.width, data.height, 0, data.format, GL_UNSIGNED_BYTE, data.data);
            glGenerateMipmap(GL_TEXTURE_2D);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            Logger.LogDebug("Texture Loader", "Loaded 2D Texture: " + std::to_string(textureID));
            return true;
        }
        else {
            Logger.LogError("Texture Loader", "Image is not loaded");
            return false;
        }
        
	}
};