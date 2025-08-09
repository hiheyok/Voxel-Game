#pragma once

#include <gl/glew.h>

#include <cstdint>
#include <glm/vec2.hpp>
#include <memory>

#include "Assets/Asset.h"

class GameContext;

/*
This is the base texture class as a base for Texture2D and Texture Atlas
 - Maybe support texture array in the future
*/
class TextureSource : public Asset {
 public:
  ~TextureSource();

  int GetWidth() const noexcept;
  int GetHeight() const noexcept;

 protected:
  // This just contain the image data and methods to access its properties
  class TextureData {
   public:
    TextureData(GameContext& context, const std::string& path);
    ~TextureData();

    TextureData(const TextureData&) = delete;

    size_t GetSize() const noexcept;
    int GetPixelCount() const noexcept;
    int GetHeight() const noexcept;
    int GetWidth() const noexcept;
    int GetFormat() const noexcept;
    int GetChannels() const noexcept;
    bool IsSuccess() const noexcept;
    const uint8_t* GetData() const noexcept;

   private:
    uint8_t* data_;
    glm::ivec2 img_size_;
    int format_;
    int channels_;
    bool success_;
  };

  explicit TextureSource(GameContext& context, const std::string& asset_key);
  std::unique_ptr<TextureData> LoadTexture(const std::string& filepath);

  GameContext& context_;
  glm::ivec2 image_size_ = {0, 0};
  int format_ = 0;
  int channels_ = 0;
};