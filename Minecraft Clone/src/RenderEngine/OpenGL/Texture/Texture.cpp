#include "RenderEngine/OpenGL/Texture/Texture.h"
#include "Utils/stb_image.h"
#include "Utils/LogUtils.h"

RawTextureData::RawTextureData() = default;

RawTextureData::RawTextureData(const char* path) {
    Load(path);
}

void RawTextureData::Erase() {
    stbi_image_free(data_);
}

void RawTextureData::Load(const char* path) {
    image_path_ = path;
    data_ = stbi_load(path, &width_, &height_, &format_, 0);

    switch (format_) {
    case 1:
        format_ = GL_RED;
        break;
    case 2:
        format_ = GL_RG;
        break;
    case 3:
        format_ = GL_RGB;
        break;
    case 4:
        format_ = GL_RGBA;
        break;
    default:
        Erase();
        g_logger.LogWarn("RawTextureData::Load", "Image invalid format");
    }
}
unsigned char RawTextureData::operator[](int index) {
    return data_[index];
}

void RawTextureData::Reload() {
    Erase();
    Load(image_path_);
}

Texture::Texture() = default;

void Texture::Gen() {
    glGenTextures(1, &texture_id_);
}

bool Texture::Load(RawTextureData data) {
    (void)data;
    return false;
}

bool Texture::Load(std::string file) {
    (void)file;
    return false;
}

GLuint Texture::get() {
    return texture_id_;
}