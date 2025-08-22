#include "Assets/Types/Texture/TextureAtlasSource.h"

#include <filesystem>
#include <nlohmann/json.hpp>

#include "Assets/Types/Texture/Atlas/Sticher.h"
#include "Core/GameContext/GameContext.h"
#include "Core/IO/FileUtils.h"
#include "Utils/LogUtils.h"

using json = nlohmann::json;
using namespace std::filesystem;
using std::move;
using std::nullopt;
using std::optional;
using std::string;
using std::vector;

TextureAtlasSource::TextureAtlasSource(GameContext& context, const string& key,
                                       const string& atlas_def)
    : Texture2DBaseSource{context, key}, atlas_def_{atlas_def} {}

void TextureAtlasSource::Load() {
  LOG_INFO("Loading texture atlas: {}", atlas_def_);
  vector<SpritePath> sprites = GetPathList();
  sprites_.reserve(sprites.size());
  // Now load in all of the images in memory
  for (auto [sprite_path, sprite_name] : sprites) {
    TextureData data = LoadTexture(sprite_path);
    SpriteData sprite;
    sprite.partial_trans_ = data.IsPartialTrans();
    sprite.full_trans_ = data.IsFullTrans();
    sprite.data_ = move(data);
    sprite.name_ = sprite_name;
    sprites_.push_back(move(sprite));
  }

  // Now stitch the data together
  Stitch();
  format_ = GetFormat(channels_);

  for (const auto& sprite : sprites_) {
    Sprite s;
    s.name_ = sprite.name_;
    s.uv_beg_.x = static_cast<float>(sprite.x_) / width_;
    s.uv_beg_.y = static_cast<float>(sprite.y_) / height_;
    s.uv_end_.x = static_cast<float>(sprite.x_ + sprite.width_) / width_;
    s.uv_end_.y = static_cast<float>(sprite.y_ + sprite.height_) / height_;
    s.partial_trans_ = sprite.partial_trans_;
    s.full_trans_ = sprite.full_trans_;
    sprites_map_.insert({ResourceLocation{sprite.name_}, s});
  }
}

vector<TextureAtlasSource::Sprite> TextureAtlasSource::GetAllSprites() const {
  vector<Sprite> sprites;
  sprites.reserve(sprites_.size());
  for (const auto& sprite : sprites_) {
    Sprite s;
    s.name_ = sprite.name_;
    s.uv_beg_.x = static_cast<float>(sprite.x_) / width_;
    s.uv_beg_.y = static_cast<float>(sprite.y_) / height_;
    s.uv_end_.x = static_cast<float>(sprite.x_ + sprite.width_) / width_;
    s.uv_end_.y = static_cast<float>(sprite.y_ + sprite.height_) / height_;
    sprites.push_back(move(s));
  }
  return sprites;
}

optional<TextureAtlasSource::Sprite> TextureAtlasSource::GetSprite(
    const ResourceLocation& location) {
  const auto& it = sprites_map_.find(location);
  if (it != sprites_map_.end()) {
    return it->second;
  } else {
    return nullopt;
  }
}

const uint8_t* TextureAtlasSource::GetData() const noexcept {
  return data_.data();
}

vector<TextureAtlasSource::SpritePath> TextureAtlasSource::GetPathList() const {
  vector<char> data = FileUtils::ReadFileToBuffer(context_, atlas_def_);
  vector<SpritePath> sprites;

  json manifest;

  try {
    manifest = json::parse(data);
  } catch (json::exception& e) {
    LOG_ERROR("Failed to parse atlas manifest at {} : {}", atlas_def_,
              e.what());
  }

  if (!manifest.contains("sources")) {
    LOG_ERROR("Atlas manifest {} doesn't contain sources", atlas_def_);
    return {};
  }

  for (const auto& source : manifest["sources"]) {
    if (!source.contains("type")) {
      LOG_WARN("Skipping source in '{}', source doesn't contain valid type",
               atlas_def_);
      continue;
    }

    string type = source["type"].get<string>();

    if (type == "minecraft:directory") {
      if (!source.contains("source") || !source.contains("prefix")) {
        LOG_WARN(
            "Atlas source at '{}' skipped due to missing source or prefix!",
            atlas_def_);
        continue;
      }
      string path = source["source"].get<string>();
      string prefix = source["prefix"].get<string>();

      ParseTypeDirectory(sprites, path, prefix);
    } else if (type == "minecraft:single") {
      if (!source.contains("resource")) {
        LOG_WARN("Atlas source at '{}' skipped due to missing resource!",
                 atlas_def_);
        continue;
      }

      string resource = source["resource"].get<string>();
      string sprite_name = source.value("sprite", "");

      ParseTypeSingle(sprites, resource, sprite_name);
    } else {
      LOG_WARN("Atlas source at '{}' with type '{}' isn't handled yet!",
               atlas_def_, type);
    }
  }

  return sprites;
}

void TextureAtlasSource::ParseTypeDirectory(vector<SpritePath>& out,
                                            string source,
                                            string prefix) const {
  // Iterate through all of the namespaces
  // Source is relative to the /texture

  // Replace all / with forwardslash

  for (char& c : source) {
    if (c == '/') {
      c = '\\';
    }
  }

  for (const auto& space : directory_iterator("assets")) {
    if (!space.is_directory()) {
      continue;
    }

    string namespace_name = space.path().filename().string();

    path texture_path = space.path() / "textures";
    path source_path = texture_path / source;

    string source_path_str = source_path.string();

    for (const auto& file : recursive_directory_iterator(source_path)) {
      if (file.is_directory()) {
        continue;
      }

      // Check extension
      if (file.path().extension() != ".png") {
        continue;
      }

      // Get the filepath relative to texture/source

      string sprite_path = file.path().string();
      string sprite_name = namespace_name + ":" + prefix +
                           file.path()
                               .lexically_relative(source_path)
                               .replace_extension()
                               .string();

      for (char& c : sprite_name) {
        if (c == '\\') {
          c = '/';
        }
      }

      out.emplace_back(sprite_path, sprite_name);
    }
  }
}

void TextureAtlasSource::ParseTypeSingle(vector<SpritePath>& out,
                                         string resource, string sprite) const {
  // Get the namespace of the atlas source
  path manifest_path = atlas_def_;
  auto it = manifest_path.begin();
  if (manifest_path.has_root_directory()) {
    ++it;
  }

  if (it == manifest_path.end()) {
    LOG_ERROR("Parsing error when parsing {}", manifest_path.string());
    return;
  }

  ++it;

  string namespace_name = it->string();
  size_t idx = resource.find(':');
  if (idx != string::npos) {
    resource = resource.substr(idx + 1, resource.size() - idx - 1);
  }

  // Now build path to resource
  string resource_path =
      "assets/" + namespace_name + "/textures/" + resource + ".png";
  string sprite_name;
  if (sprite.size()) {
    sprite_name = sprite;
  } else {
    sprite_name = namespace_name + ":" + resource;
  }

  for (char& c : sprite_name) {
    if (c == '\\') {
      c = '/';
    }
  }

  // Then check if the file actually exist

  if (exists(resource_path)) {
    out.emplace_back(resource_path, sprite_name);
  }
}

void TextureAtlasSource::Stitch() {
  // Use stitcher to stitch together all of the textures
  Stitcher stitcher{context_, kDefaultAtlasSize, kDefaultAtlasSize};

  int channels = 1;

  for (auto& sprite : sprites_) {
    TextureData& data = sprite.data_;
    const string& name = sprite.name_;

    auto [x, y] = stitcher.PlaceItem(data.GetWidth(), data.GetHeight());

    channels = std::max(channels, data.GetChannels());

    sprite.x_ = x;
    sprite.y_ = y;
    sprite.width_ = data.GetWidth();
    sprite.height_ = data.GetHeight();
  }

  width_ = stitcher.GetWidth();
  height_ = stitcher.GetHeight();
  channels_ = channels;
  // Stitch all of the final results into a single image
  data_.resize(width_ * height_ * channels_);

  for (const auto& sprite : sprites_) {
    StitchTexture(sprite);
  }
}

void TextureAtlasSource::StitchTexture(const SpriteData& data) {
  int x_offset = data.x_;
  int y_offset = data.y_;

  int x_sprite = data.width_;
  int y_sprite = data.height_;

  int sprite_channel = data.data_.GetChannels();
  const uint8_t* sprite_data = data.data_.GetData();

  for (int x = 0; x < x_sprite; ++x) {
    for (int y = 0; y < y_sprite; ++y) {
      int x_global = x + x_offset;
      int y_global = y + y_offset;

      glm::u8vec4 rgba = data.data_.GetPixel(x, y);
      SetPixel(x_global, y_global, rgba);
    }
  }
}

void TextureAtlasSource::SetPixel(int x, int y, glm::u8vec4 rgba) {
  const int x_stride = channels_;
  const int y_stride = width_ * channels_;

  const int x_data = x_stride * x;
  const int y_data = y_stride * y;

  for (int i = 0; i < channels_; ++i) {
    data_[x_data + y_data + i] = rgba[i];
  }
}
