#include "Assets/Asset.h"

Asset::Asset(const std::string& asset_key)
    : asset_key_{asset_key}, state{AssetState::kUnloaded} {}

Asset::~Asset() = default;

const std::string& Asset::GetAssetKey() const noexcept { return asset_key_; }
