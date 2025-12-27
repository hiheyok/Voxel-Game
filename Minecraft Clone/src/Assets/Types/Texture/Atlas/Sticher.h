#pragma once

#include <memory>
#include <utility>
#include <vector>

#include "Core/Typenames.h"

/*
Purpose: Designed to utilize a bin packing algorithm to stich textures together
into a compact atlas

TextureAtlas will interface with this when building

*/

class GameContext;

class PlaceHeuristic;
class SplitHeuristic;
class ResourceLocation;

struct StitchingRectangle;

class Stitcher {
 public:
  Stitcher(GameContext&, int initial_width, int initial_height, int alignment,
           bool can_expand = true);
  ~Stitcher();
  // Returns the position of the placed sprite
  std::pair<int, int> PlaceItem(const ResourceLocation&, int item_width,
                                int item_height);
  void RemoveItem(const ResourceLocation&);

  int GetWidth() const noexcept;
  int GetHeight() const noexcept;

 private:
  void Expand(int item_width, int item_height);
  void MergeBins();

  static constexpr int kMaxSize = 16384;
  GameContext& context_;
  const bool kCanExpand;

  std::vector<StitchingRectangle> free_bins_;
  std::unique_ptr<PlaceHeuristic> place_heuristic_;
  std::unique_ptr<SplitHeuristic> split_heuristic_;
  FastHashMap<ResourceLocation, StitchingRectangle> used_bins_;
  int width_, height_;
  int alignment_;
};