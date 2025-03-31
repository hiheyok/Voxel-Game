#pragma once
#include "../Texture.h"
#include <vector>
#include <optional>
#include <cctype>

class TextureAtlas : public Texture {
public:
    void UploadToGPU();

    void SetSize(int width, int height);

    void SetPixel(uint8_t r, uint8_t  g, uint8_t b, uint8_t a, size_t w, size_t h);

    void AddData(std::vector<uint8_t> data, int format, bool& transparency, bool& isSeeThrough);

    bool AddTextureToAtlasHelper(RawTextureData* data, bool& transparency, bool& isSeeThrough);

    std::optional<RawTextureData> AddTextureToAtlas(std::string file, bool& transparency, bool& isSeeThrough);

    size_t GetBlockCount() const;

    std::vector<uint8_t> texture_atlas_data_;
    const int color_space_ = 4;
    bool is_transparent_ = false;
    size_t count_ = 0;
};