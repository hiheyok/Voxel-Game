#include "Assets/Types/Models/Data/RenderableModel.h"

using baked_model::Element;
using std::vector;

RenderableModel::RenderableModel(GameContext& context, const std::string& key)
    : Asset{key}, context_{context} {}
RenderableModel::~RenderableModel() = default;

const vector<Element>& RenderableModel::GetElements() const noexcept {
  return elements;
}

bool RenderableModel::CheckAmbientOcclusion() const noexcept {
  return ambient_occlusion_;
}
