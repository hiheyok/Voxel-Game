#include "RenderEngine/RenderResources/RenderResource.h"

#include <string>

#include "RenderEngine/RenderResources/RenderResourceState.h"

using std::string;

RenderResource::RenderResource(const string& key)
    : key_{key}, state{RenderResourceState::kUnloaded} {}

RenderResource::~RenderResource() = default;

const string& RenderResource::GetKey() const noexcept { return key_; }
