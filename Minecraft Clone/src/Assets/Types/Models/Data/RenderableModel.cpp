#include "Assets/Types/Models/Data/RenderableModel.h"

#include <string>
#include <vector>

#include "Assets/Asset.h"

using baked_model::Element;
using std::string;
using std::vector;

RenderableModel::RenderableModel(GameContext& context, const string& key)
    : Asset{key}, context_{context} {}
RenderableModel::~RenderableModel() = default;

const vector<Element>& RenderableModel::GetElements() const noexcept {
  return elements;
}

bool RenderableModel::CheckAmbientOcclusion() const noexcept {
  return ambient_occlusion_;
}
