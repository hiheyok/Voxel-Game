#include "RenderEngine/RenderResources/RenderResource.h"

#include <string>

#include "RenderEngine/RenderResources/RenderResourceState.h"

RenderResource::RenderResource(const std::string& key)
    : key_{key}, state{RenderResourceState::kUnloaded} {}

RenderResource::~RenderResource() = default;

const std::string& RenderResource::GetKey() const noexcept { return key_; }
