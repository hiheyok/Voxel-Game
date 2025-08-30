#pragma once

#include <memory>
#include <vector>

#include "Assets/Types/Models/Data/RenderableModel.h"

class GameContext;
class Item;
class RenderableModel;
class RenderDrawElements;
class Camera;

// Designed to render item models
// TODO(hiheyok): Add future support for rendering items in world space

class ItemRender {
  struct VertexFormat {
    glm::vec3 pos_;
    glm::vec2 uv_;
    float light_;
  };

 public:
  ItemRender(GameContext&);
  ~ItemRender();

  ItemRender(const ItemRender&) = delete;
  ItemRender& operator=(const ItemRender&) = delete;

  ItemRender(ItemRender&&) = delete;
  ItemRender& operator=(ItemRender&&) = delete;

  void RenderModel(const Item&);

 private:
  void Initialize();
  void SetUpRenderer();
  void SetUpData();
  void SetDrawCalls();

  void GetVertices(AssetHandle<RenderableModel> model,
                   std::vector<VertexFormat>& vertices,
                   std::vector<uint32_t>& indices);

  GameContext& context_;

  // id_index_offset_[id] = index offset, model size
  std::vector<std::pair<size_t, size_t>> item_model_info_;

  std::unique_ptr<RenderDrawElements> render_;
  std::unique_ptr<Camera> camera_;
};
