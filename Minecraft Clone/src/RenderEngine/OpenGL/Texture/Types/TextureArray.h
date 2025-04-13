#pragma once
#include "RenderEngine/OpenGL/Texture/Texture.h"
#include <vector>
#include <optional>

class TextureArray : public Texture {
public:
    void LoadToGPU();

    void SetSize(int width, int height);

    void AddData(std::vector<uint8_t> data, size_t width, size_t height, int format);

    bool AddTextureToArray(RawTextureData* data);

    std::optional<RawTextureData> AddTextureToArray(std::string file);

    int GetLayers() const;

    std::vector<unsigned char> array_data_;

    int layers_ = 0;

};