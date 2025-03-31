#pragma once

#include <string>
#include <gl/glew.h>
#include <GLFW/glfw3.h>


class RawTextureData {
public:
    RawTextureData();

    RawTextureData(const char* path);

    void Erase();

    void Load(const char* path);

    unsigned char operator[](int index);

    void Reload();

    int width_ = 0;
    int height_ = 0;
    int format_ = 0;
    unsigned char* data_ = nullptr;
private:
    const char* image_path_ = nullptr;
};

class Texture {
public:
    Texture();

    void Gen();

    virtual bool Load(RawTextureData data);

    virtual bool Load(std::string file);

    GLuint get();

    GLuint texture_id_ = 0;
    size_t width_ = 0;
    size_t height_ = 0;
    int format_ = 0;
};