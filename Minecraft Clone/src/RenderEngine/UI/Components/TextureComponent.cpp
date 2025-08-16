#include "RenderEngine/UI/Components/TextureComponent.h"

#include "RenderEngine/RenderResources/Types/Texture/TextureAtlas.h"
#include "RenderEngine/UI/Data/UserInterfaceData.h"

TextureComponent::TextureComponent(TextureSprite sprite)
    : type_{TextureType::kSprite},
      sprite_{sprite},
      r_{1.0f},
      g_{1.0f},
      b_{1.0f},
      a_{1.0f} {}

TextureComponent::TextureComponent(RenderHandle<TextureV2> handle)
    : type_{TextureType::kFull},
      handle_{handle},
      r_{1.0f},
      g_{1.0f},
      b_{1.0f},
      a_{1.0f} {}

TextureComponent::~TextureComponent() = default;

void TextureComponent::GetRectangles(std::vector<UIRectangle>& primitives) {
  UIRectangle rect;
  rect.rgba_ = {r_, g_, b_, a_};
  rect.pos_.x = 0.0f;
  rect.pos_.y = 0.0f;
  rect.size_.x = 1.0f;
  rect.size_.y = 1.0f;

  if (type_ == TextureType::kSprite) {
    rect.uv_beg_ = sprite_.uv_beg_;
    rect.uv_end_ = sprite_.uv_end_;
    rect.tex_handle_ = sprite_.handle_;
  } else if (type_ == TextureType::kFull) {
    rect.uv_beg_ = {0.0f, 0.0f};
    rect.uv_end_ = {1.0f, 1.0f};
    rect.tex_handle_ = handle_->GetHandle();
  }
  
  primitives.push_back(rect);
}
