#include <algorithm>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>
#include <glm/vec2.hpp>
#include <limits>

#include "Assets/Types/Texture/Atlas/Sticher.h"
#include "Assets/Types/Texture/Atlas/StitchingHeuristics.h"
#include "Assets/Types/Texture/Atlas/StitchingRectangle.h"
#include "Core/Typenames.h"

Stitcher::Stitcher(GameContext& context, int initial_width, int initial_height)
    : context_{context},
      place_heuristic_{std::make_unique<BestShortSideFit>()},
      split_heuristic_{std::make_unique<ShorterAxisSplit>()},
      width_{initial_width},
      height_{initial_height} {
  free_bins_.emplace_back(0, 0, width_, height_);
}

Stitcher::~Stitcher() = default;

std::pair<int, int> Stitcher::PlaceItem(int item_width, int item_height) {
  while (width_ < kMaxSize || height_ < kMaxSize) {
    int idx =
        place_heuristic_->FindBestSpot(item_width, item_height, free_bins_);

    if (idx != -1) {
      // Copy the rectangle before deleting from free list
      StitchingRectangle rect = free_bins_[idx];

      // Swap and pop O(1) deletion
      std::swap(free_bins_[idx], free_bins_.back());
      free_bins_.pop_back();

      std::vector<StitchingRectangle> new_rects =
          split_heuristic_->Split(item_width, item_height, rect);

      if (!new_rects.empty()) {
        free_bins_.reserve(free_bins_.size() + new_rects.size());
        free_bins_.insert(free_bins_.end(), new_rects.begin(), new_rects.end());
      }

      MergeBins();
      return {rect.x_, rect.y_};
    } else {
      Expand(item_width, item_height);
      MergeBins();
    }
  }

  return {-1, -1};
}

int Stitcher::GetWidth() const noexcept { return width_; }

int Stitcher::GetHeight() const noexcept { return height_; }

void Stitcher::Expand(int item_width, int item_height) {
  if (width_ <= height_) {
    int old_width = width_;
    width_ *= 2;
    free_bins_.emplace_back(old_width, 0, width_ - old_width, height_);
  } else {
    int old_height = height_;
    height_ *= 2;
    free_bins_.emplace_back(0, old_height, width_, height_ - old_height);
  }
}

void Stitcher::MergeBins() {
  FastHashMap<glm::ivec2, StitchingRectangle> rects_map;
  FastHashSet<glm::ivec2> consumed_set;

  for (const auto& bin : free_bins_) {
    rects_map.emplace(glm::ivec2{bin.x_, bin.y_}, bin);
  }

  // Merges horizontally first
  for (auto& [pos, rect] : rects_map) {
    if (consumed_set.contains(pos)) {
      continue;
    }

    while (true) {
      const auto& it = rects_map.find(pos + glm::ivec2{rect.width_, 0});

      if (it == rects_map.end() || it->second.height_ != rect.height_) {
        break;  // No neighbor or mismatched height
      }

      consumed_set.insert({it->second.x_, it->second.y_});
      rect.width_ += it->second.width_;
    }
  }

  // Merges vertically
  for (auto& [pos, rect] : rects_map) {
    if (consumed_set.contains(pos)) {
      continue;
    }

    while (true) {
      const auto& it = rects_map.find(pos + glm::ivec2{0, rect.height_});

      if (it == rects_map.end() || it->second.width_ != rect.width_) {
        break;  // No neighbor or mismatched width
      }

      consumed_set.insert({it->second.x_, it->second.y_});
      rect.height_ += it->second.height_;
    }
  }

  // Then rebuild the free bins
  free_bins_.clear();
  free_bins_.reserve(rects_map.size() - consumed_set.size());
  for (const auto& [pos, bin] : rects_map) {
    if (!consumed_set.contains(pos)) {
      free_bins_.emplace_back(bin);
    }
  }
}
