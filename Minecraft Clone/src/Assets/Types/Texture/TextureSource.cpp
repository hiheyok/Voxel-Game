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

int TextureSource::GetFormat() const noexcept { return format_; }

TextureSource::TextureData TextureSource::LoadTexture(
    const std::string& filepath) {
  return {context_, filepath};
}

int TextureSource::GetFormat(int channel) noexcept {
  switch (channel) {
    case 1:
      return GL_RED;
    case 2:
      return GL_RG;
    case 3:
      return GL_RGB;
    case 4:
      return GL_RGBA;
  }
}

TextureSource::TextureData::TextureData(GameContext& context,
                                        const std::string& filepath) {
  Load(context, filepath);
  if (success_) {
    Analyze();
  }
}

void TextureSource::TextureData::Load(GameContext& context,
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

void TextureSource::TextureData::Analyze() {
  // Analyze the transparency
  if (channels_ == 4) {
    for (int i = 0; i < img_size_.x * img_size_.y; ++i) {
      int alpha = data_[i * 4 + 3];
      if (alpha > 0 && alpha < 255) {
        partial_trans_ = true;
      }

      if (alpha == 0) {
        full_trans_ = true;
      }

      if (full_trans_ && partial_trans_) {
        break;
      }
    }
  }
}

TextureSource::TextureData::~TextureData() {
  if (data_) {
    stbi_image_free(data_);
    data_ = nullptr;
  }
}

TextureSource::TextureData::TextureData() = default;

TextureSource::TextureData::TextureData(TextureData&& other) noexcept
    : data_(other.data_),
      img_size_(other.img_size_),
      format_(other.format_),
      channels_(other.channels_),
      success_(other.success_),
      partial_trans_(other.partial_trans_) {
  other.data_ = nullptr;
  other.img_size_ = {0, 0};
  other.format_ = 0;
  other.channels_ = 0;
  other.success_ = false;
  other.partial_trans_ = false;
}

TextureSource::TextureData& TextureSource::TextureData::operator=(
    TextureData&& other) noexcept {
  if (this != &other) {
    data_ = std::move(other.data_);
    img_size_ = std::move(other.img_size_);
    format_ = std::move(other.format_);
    channels_ = std::move(other.channels_);
    success_ = std::move(other.success_);
    partial_trans_ = std::move(other.partial_trans_);

    other.data_ = nullptr;
    other.img_size_ = {0, 0};
    other.format_ = 0;
    other.channels_ = 0;
    other.success_ = 0;
    other.partial_trans_ = false;
  }
  return *this;
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

bool TextureSource::TextureData::IsPartialTrans() const noexcept {
  return partial_trans_;
}

bool TextureSource::TextureData::IsFullTrans() const noexcept {
  return full_trans_;
}

glm::u8vec4 TextureSource::TextureData::GetPixel(int x, int y) const noexcept {
  int stride_x = x * channels_;
  int stride_y = y * img_size_.x * channels_;

  glm::u8vec4 rgba;
  for (int i = 0; i < channels_; ++i) {
    rgba[i] = data_[stride_x + stride_y + i];
  }

  if (channels_ != 4) {
    rgba.a = 255;
  }

  if (channels_ == 1) {
    rgba.g = rgba.r;
    rgba.b = rgba.r;
  }

  return rgba;
}

const uint8_t* TextureSource::TextureData::GetData() const noexcept {
  return data_;
}
