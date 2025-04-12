#include "RenderEngine/OpenGL/Texture/Types/TextureAtlas.h"
#include "Utils/LogUtils.h"

void TextureAtlas::UploadToGPU() {
    // GLsizei mipLevelcount_ = 4;

    glBindTexture(GL_TEXTURE_2D, texture_id_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, static_cast<GLsizei>(width_), static_cast<GLsizei>(height_), 0, GL_RGBA, GL_UNSIGNED_BYTE, texture_atlas_data_.data());
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 4);

    g_logger.LogDebug("TextureAtlas::UploadToGPU", "Loaded Texture Atlas: " + std::to_string(texture_id_));
}

void TextureAtlas::SetSize(int width, int height) {
    width_ = width;
    height_ = height;

    texture_atlas_data_.resize(width_ * height_ * color_space_, 0);
}

void TextureAtlas::SetPixel(int r, int  g, int b, int a, size_t w, size_t h) {
    texture_atlas_data_[w * color_space_ + h * color_space_ + 0] = r;
    texture_atlas_data_[w * color_space_ + h * color_space_ + 1] = g;
    texture_atlas_data_[w * color_space_ + h * color_space_ + 2] = b;
    texture_atlas_data_[w * color_space_ + h * color_space_ + 3] = a;
}

void TextureAtlas::AddData(std::vector<uint8_t> data, int format) { //assumes that all textures  are 16 x 16
    //Get offset to where to put the texture
    size_t widthTex = width_ / 16;
    //int heightTex = height / 16;

    size_t widthIndex = count_ % widthTex;
    size_t heightIndex = count_ / widthTex;

    size_t widthIndexPixel = widthIndex * 16; //Offset for the width
    size_t heightIndexPixel = width_ * heightIndex * 16; //Offset for the height

    switch (format) {
    case GL_RGB:
        for (int index = 0; index < 16 * 16; index++) {
            SetPixel(data[(index * 3)], data[(index * 3) + 1], data[(index * 3) + 2], 255, widthIndexPixel + (index % 16), heightIndexPixel + width_ * (index / 16));
        }
        count_++;
        break;
    case GL_RG:
        for (int index = 0; index < 16 * 16; index++) {
            SetPixel(data[(index * 2)], data[(index * 2)], data[(index * 2)], data[(index * 2) + 1], widthIndexPixel + (index % 16), heightIndexPixel + width_ * (index / 16));
        }
        count_++;
        break;
    case GL_RGBA:
        for (int index = 0; index < 16 * 16; index++) {
            SetPixel(data[(index * 4)], data[(index * 4) + 1], data[(index * 4) + 2], data[(index * 4) + 3], widthIndexPixel + (index % 16), heightIndexPixel + width_ * (index / 16));
        }
        count_++;
        break;
    case GL_RED:
        for (int index = 0; index < 16 * 16; index++) {
            SetPixel(data[index], data[index], data[index], 255, widthIndexPixel + (index % 16), heightIndexPixel + width_ * (index / 16));
        }
        count_++;
        break;
    default:
        g_logger.LogError("TextureAtlas::AddData", "Invalid image format!");
        break;
    }
}

bool TextureAtlas::AddTextureToAtlasHelper(RawTextureData* data) {
    format_ = GL_RGBA;
    if (!data->data_) {
        g_logger.LogWarn("TextureAtlas::AddTextureToAtlasHelper", "No texture");
        return false;
    }

    if (((data->width_ % 16) != 0) || ((data->height_ % 16) != 0)) {
        g_logger.LogError("TextureAtlas::AddTextureToAtlasHelper", "Width or height doesn't match");
        return false;
    }

    int imgsX = data->width_ / 16;
    int imgsY = data->height_ / 16;

    int colorSize = 3;

    if (data->format_ == GL_RGBA) {
        colorSize = 4;
    } else if (data->format_ == GL_RG) {
        colorSize = 2;
    } else if (data->format_ == GL_RED) {
        colorSize = 1;
    }

    int cWidth = 16 * colorSize;

    for (int x = 0; x < imgsX; x++) {
        for (int y = 0; y < imgsY; y++) {
            int gx = x * 16 * colorSize;
            int gy = y * 16 * data->width_ * colorSize;

            std::vector<uint8_t> buffer(16 * 16 * colorSize);

            for (int h = 0; h < 16; h++) {
                memcpy(buffer.data() + (h * cWidth), data->data_ + (h * data->width_ * colorSize + gx + gy), cWidth);
            }

            AddData(buffer, data->format_);
        }
    }

    return true;
}

std::optional<RawTextureData> TextureAtlas::AddTextureToAtlas(std::string file) {
    std::optional<RawTextureData> data;
    RawTextureData tex{ file };
    if (AddTextureToAtlasHelper(&tex)) {
        g_logger.LogInfo("TextureAtlas::AddTextureToAtlas", "Loaded: " + file + " | Size: " + std::to_string(tex.height_) + ", " + std::to_string(tex.width_));
        data = std::move(tex);
        return data;
    }
    g_logger.LogWarn("TextureAtlas::AddTextureToAtlas", "Unable to load: " + file);
    return data;
}

size_t TextureAtlas::GetBlockCount() const {
    return count_;
}