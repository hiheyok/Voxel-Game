#include "Assets/Types/Texture/Texture2DSource.h"

#include <cstdint>
#include <string>

#include "Assets/Types/Texture/Texture2DBaseSource.h"

using std::string;

Texture2DSource::Texture2DSource(GameContext& context, const string& asset_key,
                                 const string& path)
    : Texture2DBaseSource{context, asset_key}, filepath{path} {}

Texture2DSource::~Texture2DSource() = default;

void Texture2DSource::Load() {
  img_data_ = LoadTexture(filepath);
  width_ = img_data_.GetWidth();
  height_ = img_data_.GetHeight();
  format_ = img_data_.GetFormat();
  channels_ = img_data_.GetChannels();
}
const uint8_t* Texture2DSource::GetData() const noexcept {
  return img_data_.GetData();
}

const uint8_t* Texture2DSource::GetMipmapLayer(int layer) const noexcept {
  return nullptr;
}
