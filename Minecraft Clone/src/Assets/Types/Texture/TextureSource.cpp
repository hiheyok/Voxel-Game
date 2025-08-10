#include "Assets/Types/Texture/TextureSource.h"

#include <cstdint>
#include <vector>

#include "Core/GameContext/GameContext.h"
#include "Core/IO/FileUtils.h"
#include "Utils/LogUtils.h"
#include "Utils/stb_image.h"

TextureSource::TextureSource(GameContext& context, const std::string& asset_key)
    : Asset{asset_key}, context_{context} {}

TextureSource::~TextureSource() = default;

int TextureSource::GetWidth() const noexcept { return image_size_.x; }

int TextureSource::GetHeight() const noexcept { return image_size_.y; }

int TextureSource::GetFormat() const noexcept { return format_; }

std::unique_ptr<TextureSource::TextureData> TextureSource::LoadTexture(
    const std::string& filepath) {
  return std::make_unique<TextureData>(context_, filepath);
}

TextureSource::TextureData::TextureData(GameContext& context,
                                        const std::string& filepath) {
  std::vector<char> data = FileUtils::ReadFileToBuffer(context, filepath);

  if (data.size() == 0) {
    LOG_STATIC_ERROR(context.logger_, "Failed to load in texture data: {}",
                     filepath);
    success_ = false;
    return;
  }

  data_ = stbi_load_from_memory(reinterpret_cast<uint8_t*>(data.data()),
                                data.size(), &img_size_.x, &img_size_.y,
                                &channels_, 0);

  if (!data_) {
    LOG_STATIC_ERROR(context.logger_, "Failed to load in texture: {} | {}",
                     filepath, stbi_failure_reason());
    success_ = false;
    return;
  }

  switch (channels_) {
    case 1:
      format_ = GL_RED;
      break;
    case 2:
      format_ = GL_RG;
      break;
    case 3:
      format_ = GL_RGB;
      break;
    case 4:
      format_ = GL_RGBA;
      break;
  }

  success_ = true;

  LOG_STATIC_DEBUG(context.logger_, "Successfully loaded in texture: {}",
                   filepath);
}
TextureSource::TextureData::~TextureData() {
  if (data_) {
    stbi_image_free(data_);
  }
}

size_t TextureSource::TextureData::GetSize() const noexcept {
  assert(success_);
  return img_size_.x * img_size_.y * channels_;
}

int TextureSource::TextureData::GetPixelCount() const noexcept {
  assert(success_);
  return img_size_.x * img_size_.y;
}

int TextureSource::TextureData::GetHeight() const noexcept {
  assert(success_);
  return img_size_.y;
}

int TextureSource::TextureData::GetWidth() const noexcept {
  assert(success_);
  return img_size_.x;
}

int TextureSource::TextureData::GetFormat() const noexcept {
  assert(success_);
  return format_;
}

int TextureSource::TextureData::GetChannels() const noexcept {
  assert(success_);
  return channels_;
}

bool TextureSource::TextureData::IsSuccess() const noexcept { return success_; }

const uint8_t* TextureSource::TextureData::GetData() const noexcept {
  return data_;
}
