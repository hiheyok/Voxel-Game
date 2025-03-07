#pragma once

#include <gl/glew.h>
#include <GLFW/glfw3.h>
#include "../../../Utils/stb_image.h"
#include "../../../Utils/LogUtils.h"

class RawTextureData { //mem leak
public:

    RawTextureData() {

    }

    RawTextureData(const char* path) {
        Load(path);
    }

    void Erase() { stbi_image_free(data_); }
    void Load(const char* path) {
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
            g_logger.LogWarn("Image Loader", "Image invalid format");
        }
    }
    unsigned char operator[](int index) {
        return data_[index];
    }

    void Reload() {
        Erase();
        Load(image_path_);
    }
    int width_ = NULL;
    int height_ = NULL;
    int format_ = NULL;
    unsigned char* data_ = nullptr;
private:
    const char* image_path_ = nullptr;
};

class Texture {
public:
    void Gen() {
        glGenTextures(1, &texture_id_);
    }

    virtual bool Load(RawTextureData data) {
        (void)data;
        return false;
    }

    virtual bool Load(std::string file) {
        (void)file;
        return false;
    }

    GLuint get() {
        return texture_id_;
    }

    GLuint texture_id_ = NULL;
    size_t width_ = NULL;
    size_t height_ = NULL;
    int format_ = NULL;
};