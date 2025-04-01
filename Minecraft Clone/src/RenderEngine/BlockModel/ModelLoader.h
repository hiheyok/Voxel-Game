#pragma once
#include "RenderEngine/BlockModel/BlockModels.h"
#include <string>
#include <memory>

std::unique_ptr<Model::BlockModel> getBlockModel(std::string blockNameIn, std::string namespaceIn);