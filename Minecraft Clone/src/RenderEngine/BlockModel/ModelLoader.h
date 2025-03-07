#pragma once
#include "BlockModels.h"
#include <string>
#include <memory>

std::unique_ptr<ModelV2::BlockModelV2> getBlockModel(std::string blockNameIn, std::string namespaceIn);