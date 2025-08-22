#pragma once

#include <optional>
#include <vector>

#include "Assets/Types/Texture/Texture2DBaseSource.h"
#include "Core/Typenames.h"

class ResourceLocation;

class TextureAtlasSource : public Texture2DBaseSource {
 public:
  struct Sprite {
    std::string name_;
    glm::vec2 uv_beg_;
    glm::vec2 uv_end_;
    bool partial_trans_ = false;
    bool full_trans_ = false;
  };

  TextureAtlasSource(GameContext& context, const std::string& key,
                     const std::string& atlas_def);

  void Load() override;

  std::vector<Sprite> GetAllSprites() const;
  std::optional<Sprite> GetSprite(const ResourceLocation& location);
  const uint8_t* GetData() const noexcept override;

 private:
  struct SpritePath {
    std::string path_;
    std::string name_;
  };

  struct SpriteData {
    std::string name_;
    int x_, y_;
    int width_, height_;
    TextureData data_;
    bool partial_trans_;
    bool full_trans_;
  };

  std::vector<SpritePath> GetPathList() const;
  void ParseTypeDirectory(std::vector<SpritePath>&, std::string source,
                          std::string prefix) const;
  void ParseTypeSingle(std::vector<SpritePath>&, std::string resource,
                       std::string sprite) const;

  void Stitch();
  void StitchTexture(const SpriteData& data);
  void SetPixel(int x, int y, glm::u8vec4 rgba);

  static constexpr int kDefaultAtlasSize = 128;

  std::string atlas_def_;

  std::vector<uint8_t> data_;
  std::vector<SpriteData> sprites_;
  FastHashMap<ResourceLocation, Sprite> sprites_map_;
};
