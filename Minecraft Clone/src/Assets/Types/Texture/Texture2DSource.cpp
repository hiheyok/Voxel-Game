#include "Assets/Types/Texture/Texture2DSource.h"

Texture2DSource::Texture2DSource(GameContext& context,
                                 const std::string& asset_key,
                                 const std::string& path)
    : TextureSource{context, asset_key}, filepath{path} {}

Texture2DSource::~Texture2DSource() = default;

void Texture2DSource::Load() {
  img_data_ = LoadTexture(filepath);
  image_size_ = {img_data_->GetWidth(), img_data_->GetHeight()};
  format_ = img_data_->GetFormat();
  channels_ = img_data_->GetChannels();
}
const uint8_t* Texture2DSource::GetData() const noexcept {
  return img_data_->GetData();
}
