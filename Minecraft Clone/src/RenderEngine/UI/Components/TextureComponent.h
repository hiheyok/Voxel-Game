#pragma once

#include <cassert>
#include <vector>

#include "RenderEngine/RenderResources/RenderHandle.h"
#include "RenderEngine/RenderResources/Types/Texture/TextureSprite.h"
#include "RenderEngine/UI/Components/Component.h"

class TextureV2;
class RenderTargetTexture;

struct UIRectangle;

// Support full texture and sprites
class TextureComponent : public Component {
 public:
  explicit TextureComponent(TextureSprite sprite);
  explicit TextureComponent(RenderHandle<TextureV2> handle);
  explicit TextureComponent(RenderHandle<RenderTargetTexture> handle);
  ~TextureComponent() override;

  void GetRectangles(std::vector<UIRectangle>& primitives) override;

 private:
  enum class TextureType { kSprite, kFull, kRenderTarget };
  TextureType type_;
  TextureSprite sprite_;
  RenderHandle<TextureV2> handle_;
  RenderHandle<RenderTargetTexture> handle_render_target_;
  float r_, g_, b_, a_;
};
