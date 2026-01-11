// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once

#include <memory>
#include <vector>

#include "Client/Inputs/InputEvent.h"
#include "RenderEngine/UI/Data/UserInterfaceData.h"

class InputManager;
class Component;
class ScreenManager;
class UIRenderer;
class GameContext;

struct UIRectangle;

class Widget {
 public:
  Widget(GameContext&);
  virtual ~Widget();

  Widget(const Widget&) = delete;
  Widget& operator=(const Widget&) = delete;
  Widget(Widget&&) noexcept = delete;
  Widget& operator=(Widget&&) noexcept = delete;

  // Properties setter
  void SetPivot(glm::vec2 pos) noexcept;
  void SetAnchorBoth(glm::vec2 pos) noexcept;
  void SetAnchorMax(glm::vec2 pos) noexcept;
  void SetAnchorMin(glm::vec2 pos) noexcept;
  void SetOffsetMax(glm::vec2 pos) noexcept;
  void SetOffsetMin(glm::vec2 pos) noexcept;

  void AddChildWidget(std::unique_ptr<Widget> widget);
  void AddComponent(std::unique_ptr<Component> component);
  void TryUpdateLayout(const UIRectangle& parent);
  void SetBranchDirty() noexcept;

  void SubmitToRenderer(UIRenderer& renderer) const;
  bool OnEvent(const InputManager&);
  void AddCallback();

  void SetDirty();

 protected:
  GameContext& context_;

  // Units are in normalized coordinates [0, 1]
  glm::vec2 anchor_min_ = {0.5f, 0.5f};
  glm::vec2 anchor_max_ = {0.5f, 0.5f};
  glm::vec2 pivot_ = {0.5f, 0.5f};
  // Units are in virtual pixels
  glm::vec2 offset_min_ = {0.0f, 0.0f};  // Often called "position"
  glm::vec2 offset_max_ = {0.0f, 0.0f};  // Often called "size"

  UIRectangle screen_;

  std::vector<std::unique_ptr<Component>> components_;

  // Virtual so derived widgets can add their own primitives
  virtual void GetPrimitives(std::vector<UIRectangle>& primitives) const;

 private:
  friend class Screen;

  void SetChildDirty();
  void SetScreenManager(ScreenManager* manager) noexcept;


  ScreenManager* manager_;
  Widget* parent_;

  std::vector<std::unique_ptr<Widget>> children_;

  bool self_dirty_, children_dirty_;
};
