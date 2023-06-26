#pragma once

#define TEXTURE_H
#ifndef TETXURE_H

#include <gl/glew.h>
#include <GLFW/glfw3.h>
#include "../../../Utils/stb_image.h"
#include "../../../Utils/LogUtils.h"

class RawTextureData {
public:
    
    void Erase() { stbi_image_free(data); }
    void Load(const char* path) {
        ImagePath = path;
        data = stbi_load(path, &width, &height, &format, 0);

        switch (format) {
        case 1:
            format = GL_RED;
            break;
        case 3:
            format = GL_RGB;
            break;
        case 4:
            format = GL_RGBA;
            break;
        default:
            Erase();
            getLogger()->LogWarn("Image Loader", "Image invalid format");
        }
    }
    unsigned char operator[](int index) {
        return data[index];
    }

    void Reload() {
        Erase();
        Load(ImagePath);
    }
    int width = NULL;
    int height = NULL;
    int format = NULL;
    unsigned char* data = nullptr;
private:
    
    
    const char* ImagePath;
};

class Texture {
public:
    void Gen() {
        glGenTextures(1, &textureID);
    }

    virtual bool Load(RawTextureData data) {
        return false;
    }

    virtual bool Load(std::string file) {
        return false;
    }

    GLuint get() {
        return textureID;
    }

    GLuint textureID = NULL;

    int width = NULL;
    int height = NULL;
    int Format = NULL;
private:
    
};

#endif