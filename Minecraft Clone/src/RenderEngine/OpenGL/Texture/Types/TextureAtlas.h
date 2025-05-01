#pragma once
#include <cctype>
#include <optional>
#include <vector>

#include "RenderEngine/OpenGL/Texture/Texture.h"

class TextureAtlas : public Texture {
 public:
  TextureAtlas(int width, int height, int individualWidth,
               int individualHeight);
  virtual ~TextureAtlas();
  void LoadToGPU();

  std::optional<RawTextureData> AddTextureToAtlas(std::string file);

  size_t GetTextureCount() const;

 private:
  void SetPixel(int r, int g, int b, int a, size_t w, size_t h);
  bool AddTextureToAtlasHelper(const RawTextureData& data);
  void AddData(std::vector<uint8_t> data, int format);

  std::vector<uint8_t> texture_atlas_data_;
  size_t individual_tex_height_ = 0;
  size_t individual_tex_width_ = 0;
  const int color_space_ = 4;
  size_t count_ = 0;
};
