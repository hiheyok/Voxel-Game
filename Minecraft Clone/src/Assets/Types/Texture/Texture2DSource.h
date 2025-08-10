#pragma once

#include <memory>
#include <string>

#include "Assets/Types/Texture/TextureSource.h"

class GameContext;
/*
RenderResourceManager will take this handle and use it to upload the texture to
the GPU
*/
class Texture2DSource : public TextureSource {
 public:
  Texture2DSource(GameContext& context, const std::string& asset_key,
                  const std::string& path);
  ~Texture2DSource();

  void Load() override;

  const uint8_t* GetData() const noexcept override;

 private:
  std::unique_ptr<TextureSource::TextureData> img_data_;
  std::string filepath;
};
