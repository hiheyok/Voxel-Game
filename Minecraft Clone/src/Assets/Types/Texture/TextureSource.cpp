#include "Assets/Types/Texture/TextureSource.h"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <glm/vec4.hpp>
#include <string>
#include <utility>
#include <vector>

#include "Assets/Asset.h"
#include "Core/GameContext/GameContext.h"
#include "Core/IO/FileUtils.h"
#include "Utils/LogUtils.h"
#include "Utils/stb_image.h"

using std::move;
using std::string;
using std::vector;

TextureSource::TextureSource(GameContext& context, const string& asset_key)
    : Asset{asset_key}, context_{context} {}

TextureSource::~TextureSource() = default;

int TextureSource::GetFormat() const noexcept { return format_; }

int TextureSource::GetMipmapLayerCount() const noexcept {
  return mipmap_layer_count_;
}

TextureSource::TextureData TextureSource::LoadTexture(const string& filepath) {
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
    default:
      return GL_RGBA;
  }
}

TextureSource::TextureData::TextureData(GameContext& context,
                                        const string& filepath) {
  Load(context, filepath);
  if (success_) {
    Analyze();
  }
}

void TextureSource::TextureData::Load(GameContext& context,
                                      const string& filepath) {
  vector<char> data = FileUtils::ReadFileToBuffer(context, filepath);

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
  // Analyze the transparency - optimized for cache-friendly access
  // Only images with alpha channel need analysis (2 = grayscale+alpha, 4 = RGBA)
  if (channels_ != 2 && channels_ != 4) {
    return;
  }

  const size_t total_pixels = static_cast<size_t>(img_size_.x) * img_size_.y;
  const uint8_t* ptr = data_;

  // Alpha is at index 1 for 2-channel (grayscale+alpha) and index 3 for RGBA
  const int alpha_offset = channels_ - 1;

  for (size_t i = 0; i < total_pixels; ++i) {
    const uint8_t alpha = ptr[i * channels_ + alpha_offset];

    if (alpha == 0) {
      full_trans_ = true;
    } else if (alpha < 255) {
      partial_trans_ = true;
    }

    // Early exit when both flags are set
    if (full_trans_ && partial_trans_) {
      return;
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
    data_ = move(other.data_);
    img_size_ = move(other.img_size_);
    format_ = move(other.format_);
    channels_ = move(other.channels_);
    success_ = move(other.success_);
    partial_trans_ = move(other.partial_trans_);

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
  const int stride = (y * img_size_.x + x) * channels_;

  glm::u8vec4 rgba{0, 0, 0, 255};

  switch (channels_) {
    case 1:  // Grayscale - copy to RGB, alpha = 255
      rgba.r = data_[stride];
      rgba.g = rgba.r;
      rgba.b = rgba.r;
      break;
    case 2:  // Grayscale + Alpha
      rgba.r = data_[stride];
      rgba.g = rgba.r;
      rgba.b = rgba.r;
      rgba.a = data_[stride + 1];
      break;
    case 3:  // RGB - alpha = 255
      rgba.r = data_[stride];
      rgba.g = data_[stride + 1];
      rgba.b = data_[stride + 2];
      break;
    case 4:  // RGBA
      rgba.r = data_[stride];
      rgba.g = data_[stride + 1];
      rgba.b = data_[stride + 2];
      rgba.a = data_[stride + 3];
      break;
  }

  return rgba;
}

const uint8_t* TextureSource::TextureData::GetData() const noexcept {
  return data_;
}
