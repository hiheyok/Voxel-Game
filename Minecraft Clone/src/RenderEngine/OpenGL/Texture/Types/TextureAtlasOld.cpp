// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "RenderEngine/OpenGL/Texture/Types/TextureAtlasOld.h"

#include <string>
#include <utility>
#include <vector>

#include "Core/GameContext/GameContext.h"
#include "Utils/LogUtils.h"

TextureAtlas::TextureAtlas(GameContext& context, int width, int height,
                           int individualWidth, int individualHeight)
    : Texture{context} {
  width_ = width;
  height_ = height;
  individual_tex_height_ = individualHeight;
  individual_tex_width_ = individualWidth;

  texture_atlas_data_.resize(width_ * height_ * color_space_, 0);
}

TextureAtlas::~TextureAtlas() = default;

void TextureAtlas::LoadToGPU() {
  glBindTexture(GL_TEXTURE_2D, texture_id_);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, static_cast<GLsizei>(width_),
               static_cast<GLsizei>(height_), 0, GL_RGBA, GL_UNSIGNED_BYTE,
               texture_atlas_data_.data());
  glGenerateMipmap(GL_TEXTURE_2D);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_NEAREST_MIPMAP_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 4);

  LOG_DEBUG("Loaded Texture Atlas: {}", texture_id_);
}

void TextureAtlas::SetPixel(int r, int g, int b, int a, size_t w, size_t h) {
  texture_atlas_data_[w * color_space_ + h * color_space_ + 0] = r;
  texture_atlas_data_[w * color_space_ + h * color_space_ + 1] = g;
  texture_atlas_data_[w * color_space_ + h * color_space_ + 2] = b;
  texture_atlas_data_[w * color_space_ + h * color_space_ + 3] = a;
}

void TextureAtlas::AddData(
    std::vector<uint8_t> data,
    int format) {  // assumes that all textures  are 16 x 16
  // Get offset to where to put the texture
  size_t widthTex = width_ / individual_tex_width_;
  int heightTex = height_ / individual_tex_height_;

  size_t widthIndex = count_ % widthTex;
  size_t heightIndex = count_ / widthTex;

  size_t widthOffset =
      widthIndex * individual_tex_width_;  // Offset for the width
  size_t heightOffset =
      width_ * heightIndex * individual_tex_height_;  // Offset for the height

  switch (format) {
    case GL_RGB:
      for (int index = 0;
           index < individual_tex_height_ * individual_tex_width_; index++) {
        SetPixel(data[(index * 3)], data[(index * 3) + 1],
                 data[(index * 3) + 2], 255,
                 widthOffset + (index % individual_tex_width_),
                 heightOffset + width_ * (index / individual_tex_width_));
      }
      count_++;
      break;
    case GL_RG:
      for (int index = 0;
           index < individual_tex_height_ * individual_tex_width_; index++) {
        SetPixel(data[(index * 2)], data[(index * 2)], data[(index * 2)],
                 data[(index * 2) + 1],
                 widthOffset + (index % individual_tex_width_),
                 heightOffset + width_ * (index / individual_tex_width_));
      }
      count_++;
      break;
    case GL_RGBA:
      for (int index = 0;
           index < individual_tex_height_ * individual_tex_width_; index++) {
        SetPixel(data[(index * 4)], data[(index * 4) + 1],
                 data[(index * 4) + 2], data[(index * 4) + 3],
                 widthOffset + (index % individual_tex_width_),
                 heightOffset + width_ * (index / individual_tex_width_));
      }
      count_++;
      break;
    case GL_RED:
      for (int index = 0;
           index < individual_tex_height_ * individual_tex_width_; index++) {
        SetPixel(data[index], data[index], data[index], 255,
                 widthOffset + (index % individual_tex_width_),
                 heightOffset + width_ * (index / individual_tex_width_));
      }
      count_++;
      break;
    default:
      LOG_ERROR("Invalid image format!");
      break;
  }
}

bool TextureAtlas::AddTextureToAtlasHelper(const RawTextureData& data) {
  format_ = GL_RGBA;
  if (!data.data_) {
    LOG_WARN("No texture");
    return false;
  }

  if (((data.width_ % individual_tex_width_) != 0) ||
      ((data.height_ % individual_tex_height_) != 0)) {
    LOG_ERROR("Width or height doesn't match");
    return false;
  }

  int imgsX = data.width_ / individual_tex_height_;
  int imgsY = data.height_ / individual_tex_width_;

  int colorSize = 3;

  if (data.format_ == GL_RGBA) {
    colorSize = 4;
  } else if (data.format_ == GL_RG) {
    colorSize = 2;
  } else if (data.format_ == GL_RED) {
    colorSize = 1;
  }

  int cWidth = 16 * colorSize;

  for (int x = 0; x < imgsX; x++) {
    for (int y = 0; y < imgsY; y++) {
      int gx = x * individual_tex_width_ * colorSize;
      int gy = y * individual_tex_width_ * data.width_ * colorSize;

      std::vector<uint8_t> buffer(individual_tex_height_ *
                                  individual_tex_width_ * colorSize);

      for (int h = 0; h < individual_tex_height_; h++) {
        memcpy(buffer.data() + (h * cWidth),
               data.data_ + (h * data.width_ * colorSize + gx + gy), cWidth);
      }

      AddData(buffer, data.format_);
    }
  }

  return true;
}

std::optional<RawTextureData> TextureAtlas::AddTextureToAtlas(
    std::string file) {
  std::optional<RawTextureData> data;
  RawTextureData tex{file};
  if (AddTextureToAtlasHelper(tex)) {
    LOG_INFO("Loaded: {} | Size: {} x {}", file, tex.height_, tex.width_);
    data = std::move(tex);
    return data;
  } else {
    LOG_WARN("Unable to load: {}", file);
    return data;
  }
}

size_t TextureAtlas::GetTextureCount() const { return count_; }
