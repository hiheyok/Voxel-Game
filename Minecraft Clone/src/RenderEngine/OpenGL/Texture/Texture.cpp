#include "RenderEngine/OpenGL/Texture/Texture.h"
#include "Utils/stb_image.h"
#include "Utils/LogUtils.h"

RawTextureData::~RawTextureData() {
    Erase();
}

RawTextureData::RawTextureData(const std::string& path) {
    Load(path);
}

RawTextureData::RawTextureData(RawTextureData&& other) noexcept {
    *this = std::move(other);
}

RawTextureData& RawTextureData::operator=(RawTextureData&& other) noexcept {
    active_ = other.active_;
    data_ = other.data_;
    width_ = other.width_;
    height_ = other.height_;
    format_ = other.format_;
    partially_transparent_pixels_ = other.partially_transparent_pixels_;
    fully_transparent_pixels_ = other.fully_transparent_pixels_;

    other.data_ = nullptr;
    other.active_ = false;
    other.width_ = 0;
    other.height_ = 0;
    other.format_ = 0;
    other.partially_transparent_pixels_ = 0;
    other.fully_transparent_pixels_ = 0;

    return *this;
}

void RawTextureData::Erase() {
    if (active_) {
        stbi_image_free(data_);
        active_ = false;
    }
}

void RawTextureData::Load(const std::string& path) {
    image_path_ = path;
    data_ = stbi_load(path.c_str(), &width_, &height_, &format_, 0);
    active_ = true;
    bool success = false;

    switch (format_) {
    case 1:
        format_ = GL_RED;
        success = true;
        break;
    case 2:
        format_ = GL_RG;
        success = true;
        break;
    case 3:
        format_ = GL_RGB;
        success = true;
        break;
    case 4:
        format_ = GL_RGBA;
        success = true;
        break;
    default:
        Erase();
        g_logger.LogWarn("RawTextureData::Load", "Image invalid format");
    }
    if (success) {
        fully_transparent_pixels_ = false;
        partially_transparent_pixels_ = false;
        AnalyzeTransparency();
        g_logger.LogDebug("RawTextureData::Load", "Loaded image: " + std::string(path));
    }
}

bool RawTextureData::HasFullyTransparentPixels() const {
    return fully_transparent_pixels_;
}

bool RawTextureData::HasPartiallyTransparentPixels() const {
    return partially_transparent_pixels_;
}

void RawTextureData::AnalyzeTransparency() {
    if (format_ != GL_RGBA && format_ != GL_RG) return;

    int spacing;
    if (format_ == GL_RGBA) {
        spacing = 4;
    } else {
        spacing = 2;
    }

    int offset = spacing - 1;

    // terminates loop early if it found both fully and partially transparent pixels
    for (int i = 0; i < width_ * height_ && (!partially_transparent_pixels_ || !fully_transparent_pixels_); ++i) {
        int index = i * spacing + offset;
        
        if (data_[index] != 255 && data_[index] != 0) {
            partially_transparent_pixels_ = true;
        } else if (data_[index] == 0) {
            fully_transparent_pixels_ = true;
        }
    }
}

unsigned char RawTextureData::operator[](int index) const {
    return data_[index];
}

void RawTextureData::Reload() {
    Erase();
    Load(image_path_);
}

Texture::Texture() {
    glGenTextures(1, &texture_id_);
}

Texture::Texture(Texture&& other) noexcept {
    *this = std::move(other);
}

Texture& Texture::operator=(Texture&& other) noexcept {
    texture_id_ = other.texture_id_;
    width_ = other.width_;
    height_ = other.height_;
    format_ = other.format_;

    other.texture_id_ = 0;
    other.width_ = 0;
    other.height_ = 0;
    other.format_ = 0;

    return *this;
}

Texture::~Texture() {
    if (texture_id_ != 0) {
        glDeleteTextures(1, &texture_id_);
    }
}

void Texture::Set(int textureId, size_t height, size_t width) {
    //glDeleteTextures(1, &texture_id_);
    texture_id_ = textureId;
    height_ = height;
    width_ = width;
}

GLuint Texture::Get() const {
    return texture_id_;
}

size_t Texture::GetHeight() const {
    return height_;
}

size_t Texture::GetWidth() const {
    return width_;
}

int Texture::GetFormat() const {
    return format_;
}