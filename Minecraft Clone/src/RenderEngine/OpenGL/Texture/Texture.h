// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once

#include <gl/glew.h>

#include <string>

class GameContext;

class RawTextureData {
 public:
  ~RawTextureData();
  explicit RawTextureData(const std::string& path);
  RawTextureData(RawTextureData&&) noexcept;
  RawTextureData(const RawTextureData&) = delete;

  RawTextureData& operator=(const RawTextureData&) = delete;
  RawTextureData& operator=(RawTextureData&&) noexcept;

  uint8_t operator[](int index) const;

  void Erase();
  void Load(const std::string& path);
  void Reload();

  bool HasFullyTransparentPixels() const;
  bool HasPartiallyTransparentPixels() const;

  int width_ = 0;
  int height_ = 0;
  int format_ = 0;
  uint8_t* data_ = nullptr;

 private:
  void AnalyzeTransparency();

  std::string image_path_;
  bool active_ = false;
  bool partially_transparent_pixels_ = false;
  bool fully_transparent_pixels_ = false;
};

// Abstract class to handle textures
class Texture {
 public:
  explicit Texture(GameContext&);
  Texture(const Texture&) = delete;
  Texture(Texture&&) noexcept;
  virtual ~Texture();

  Texture& operator=(Texture&&) noexcept;

  void Set(int textureId, size_t height,
           size_t width);  // TODO(hiheyok): tmp function for now
  GLuint Get() const;
  size_t GetHeight() const;
  size_t GetWidth() const;
  int GetFormat() const;

 protected:
  GameContext& game_context_;
  GLuint texture_id_ = 0;
  size_t width_ = 0;
  size_t height_ = 0;
  int format_ = 0;
};
