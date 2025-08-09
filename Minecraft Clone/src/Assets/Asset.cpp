#include "Assets/Asset.h"

Asset::Asset(const std::string& key)
    : key_{key}, state{AssetState::kUnloaded} {}

Asset::~Asset() = default;

const std::string& Asset::GetKey() const noexcept { return key_; }
