#pragma once
#include <vector>
#include <optional>
#include <cctype>

#include "RenderEngine/OpenGL/Texture/Texture.h"

class TextureAtlas : public Texture {
public:
    void UploadToGPU();

    void SetSize(int width, int height);

    void SetPixel(int r, int  g, int b, int a, size_t w, size_t h);

    void AddData(std::vector<uint8_t> data, int format);

    bool AddTextureToAtlasHelper(RawTextureData* data);

    std::optional<RawTextureData> AddTextureToAtlas(std::string file);

    size_t GetBlockCount() const;

    std::vector<uint8_t> texture_atlas_data_;
    const int color_space_ = 4;
    bool is_transparent_ = false;
    size_t count_ = 0;
};