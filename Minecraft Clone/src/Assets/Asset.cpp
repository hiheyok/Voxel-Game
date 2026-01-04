#include "Assets/Asset.h"

#include <string>

#include "Assets/AssetState.h"

using std::string;

Asset::Asset(const string& key) : key_{key}, state{AssetState::kUnloaded} {}

Asset::~Asset() = default;

const string& Asset::GetKey() const noexcept { return key_; }
