#pragma once

#include <vector>

/*
The purpose of these is to give a modular ways to swap out heuristics for later
optimizations
*/

struct StitchingRectangle;

class PlaceHeuristic {
 public:
  // Returns the index of the best spot. returns -1 if none exist
  virtual int FindBestSpot(
      int item_width, int item_height,
      const std::vector<StitchingRectangle>& free_bins) const = 0;
};

class SplitHeuristic {
 public:
  // returns a list of rectangles after splitting
  virtual std::vector<StitchingRectangle> Split(
      int item_width, int item_height, const StitchingRectangle& rect) const = 0;
};

// Finds the spot that leaves the smallest leftover side. Often the best choice
class BestShortSideFit : public PlaceHeuristic {
 public:
  int FindBestSpot(
      int item_width, int item_height,
      const std::vector<StitchingRectangle>& free_rects) const override;
};

// Splits the remaining space along its shorter axis
class ShorterAxisSplit : public SplitHeuristic {
 public:
  std::vector<StitchingRectangle> Split(
      int item_width, int item_height,
      const StitchingRectangle& rect) const override;
};
