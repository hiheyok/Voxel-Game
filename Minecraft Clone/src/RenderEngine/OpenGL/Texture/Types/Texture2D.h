#pragma once
#include "../Texture.h"
#include "../../../../Utils/LogUtils.h"

class Texture2D : public Texture {
public:
    Texture2D(RawTextureData data) {
        Gen();
        Load(data);
    }

    Texture2D() {
    }

    bool Load(RawTextureData data) override {
        if (data.data_) {
            glBindTexture(GL_TEXTURE_2D, texture_id_);
            glTexImage2D(GL_TEXTURE_2D, 0, data.format_, data.width_, data.height_, 0, data.format_, GL_UNSIGNED_BYTE, data.data_);
            glGenerateMipmap(GL_TEXTURE_2D);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            g_logger.LogDebug("Texture Loader", "Loaded 2D Texture: " + std::to_string(texture_id_));
            width_ = data.width_;
            height_ = data.height_;
            return true;
        }
        else {
            g_logger.LogError("Texture Loader", "Image is not loaded");
            return false;
        }
        
    }
};