#include "RenderEngine/OpenGL/Texture/Types/TextureArray.h"
#include "Utils/LogUtils.h"

void TextureArray::UploadToGPU() {
    GLsizei mipLevelCount = 4;

    glBindTexture(GL_TEXTURE_2D_ARRAY, texture_id_);
    glTexStorage3D(GL_TEXTURE_2D_ARRAY, mipLevelCount, GL_RGBA8, 16, 16, layers_);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0, static_cast<GLsizei>(width_), static_cast<GLsizei>(height_), layers_, GL_RGBA, GL_UNSIGNED_BYTE, array_data_.data());
    glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
    g_logger.LogDebug("TextureArray::UploadToGPU", "Loaded Texture Array: " + std::to_string(texture_id_));
}

void TextureArray::SetSize(int width, int height) {
    width_ = width;
    height_ = height;
}

void TextureArray::AddData(std::vector<uint8_t> data, size_t width, size_t height, int format) {
    if (format == GL_RGB) {
        for (size_t index = 0; index < width * height; index++) {
            array_data_.push_back(data[(index * 3)]);
            array_data_.push_back(data[(index * 3) + 1]);
            array_data_.push_back(data[(index * 3) + 2]);
            array_data_.push_back(255);
        }
        layers_++;
    }
    else  if (format == GL_RGBA) {
        for (size_t index = 0; index < width * height; index++) {
            array_data_.push_back(data[(index * 4)]);
            array_data_.push_back(data[(index * 4) + 1]);
            array_data_.push_back(data[(index * 4) + 2]);
            array_data_.push_back(data[(index * 4) + 3]);
        }
        layers_++;
    }
    else  if (format == GL_RG) {
        for (size_t index = 0; index < width * height; index++) {
            array_data_.push_back(data[(index * 2)]);
            array_data_.push_back(data[(index * 2)]);
            array_data_.push_back(data[(index * 2)]);
            array_data_.push_back(data[(index * 2) + 1]);
        }
        layers_++;
    }
    else if (format == GL_RED) {
        for (size_t index = 0; index < width * height; index++) {
            array_data_.push_back(data[index]);
            array_data_.push_back(data[index]);
            array_data_.push_back(data[index]);
            array_data_.push_back(255);
        }
        layers_++;
    }
    else {
        throw std::runtime_error("Invalid texture type not handled");
    }
}

bool TextureArray::AddTextureToArray(RawTextureData* data) {
    format_ = GL_RGBA;
    if (!data->data_) {
        g_logger.LogError("TextureArray::AddTextureToArray", "No texture");
        return false;
    }

    if (((data->width_ % width_) != 0) || ((data->height_ % height_) != 0)) {
        g_logger.LogError("TextureArray::AddTextureToArray", "Width or height doesn't match");
        return false;
    }

    size_t imgsX = data->width_ / width_;
    size_t imgsY = data->height_ / height_;

    int colorSize = 3;

    if (data->format_ == GL_RGBA) {
        colorSize = 4;
    }
    if (data->format_ == GL_RG) {
        colorSize = 2;
    }
    else if (data->format_ == GL_RED) {
        colorSize = 1;
    }

    size_t cWidth = width_ * colorSize;

    for (size_t x = 0; x < imgsX; x++) {
        for (size_t y = 0; y < imgsY; y++) {
            size_t gx = x * width_ * colorSize;
            size_t gy = y * height_ * data->width_ * colorSize;

            std::vector<uint8_t> buffer(width_ * height_ * colorSize);

            for (size_t h = 0; h < height_; h++) {
                memcpy(buffer.data() + (h * cWidth), data->data_ + (h * data->width_ * colorSize + gx + gy), cWidth);
            }

            AddData(buffer, width_, height_, data->format_);
        }
    }

    return true;
}
std::optional<RawTextureData> TextureArray::AddTextureToArray(std::string file) {
    std::optional<RawTextureData> data;
    RawTextureData tex{ file };
    if (AddTextureToArray(&tex)) {
        g_logger.LogInfo("TextureArray::AddTextureToArray", "Loaded: " + file + " | Size: " + std::to_string(tex.height_) + ", " + std::to_string(tex.width_));
        data = std::move(tex);
        return data;
    } else {
        g_logger.LogError("TextureArray::AddTextureToArray", "Unable to load: " + file);
        return data;
    }
}

int TextureArray::GetLayers() const {
    return layers_;
}