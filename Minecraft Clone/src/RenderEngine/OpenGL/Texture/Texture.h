#pragma once

#include <string>
#include <gl/glew.h>
#include <GLFW/glfw3.h>


class RawTextureData {
public:
    ~RawTextureData();
    RawTextureData(const std::string& path);
    RawTextureData(RawTextureData&&) noexcept;
    RawTextureData(const RawTextureData&) = delete;

    RawTextureData& operator=(const RawTextureData&) = delete;
    RawTextureData& operator=(RawTextureData&&) noexcept;

    unsigned char operator[](int index) const;

    void Erase();
    void Load(const std::string& path);
    void Reload();

    bool HasFullyTransparentPixels() const;
    bool HasPartiallyTransparentPixels() const;

    int width_ = 0;
    int height_ = 0;
    int format_ = 0;
    unsigned char* data_ = nullptr;
private:
    void AnalyzeTransparency();

    std::string image_path_;
    bool active_ = false;
    bool partially_transparent_pixels_ = false;
    bool fully_transparent_pixels_ = false;
};

// Abstract class to handle textures
class Texture {
public:
    Texture();
    Texture(const Texture&) = delete;
    Texture(Texture&&) noexcept;
    virtual ~Texture();

    Texture& operator=(Texture&&) noexcept;

    void Set(int textureId, size_t height, size_t width); // TODO: tmp function for now
    GLuint Get() const;
    size_t GetHeight() const;
    size_t GetWidth() const;
    int GetFormat() const;

protected:
    GLuint texture_id_ = 0;
    size_t width_ = 0;
    size_t height_ = 0;
    int format_ = 0;
};