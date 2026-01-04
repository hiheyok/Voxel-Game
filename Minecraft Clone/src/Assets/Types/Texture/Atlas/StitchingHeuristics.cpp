#include "Assets/Types/Texture/Atlas/StitchingHeuristics.h"

#include <algorithm>
#include <limits>
#include <vector>

#include "Assets/Types/Texture/Atlas/StitchingRectangle.h"

using std::min;
using std::numeric_limits;
using std::vector;

int BestShortSideFit::FindBestSpot(
    int item_width, int item_height, int alignment,
    const vector<StitchingRectangle>& free_rects) const {
  int best_bin_idx = -1;
  int best_score = numeric_limits<int>::max();

  for (int i = 0; i < free_rects.size(); ++i) {
    const StitchingRectangle& rect = free_rects[i];
    if (rect.width_ >= item_width && rect.height_ >= item_height) {
      int leftoverWidth = rect.width_ - item_width;
      int leftoverHeight = rect.height_ - item_height;
      int score = min(leftoverWidth, leftoverHeight);

      if (score < best_score) {
        best_score = score;
        best_bin_idx = i;
      }
    }
  }

  return best_bin_idx;
}

vector<StitchingRectangle> ShorterAxisSplit::Split(
    int item_width, int item_height, int alignment,
    const StitchingRectangle& rect) const {
  vector<StitchingRectangle> new_rects;
  int remaining_width = rect.width_ - item_width;
  int remaining_height = rect.height_ - item_height;

  // Split along the shorter axis to create more square-like new rectangles
  if (rect.width_ < rect.height_) {
    // Vertical cut first
    if (remaining_width > 0) {
      new_rects.emplace_back(rect.x_ + item_width, rect.y_, remaining_width,
                             rect.height_);
    }
    if (remaining_height > 0) {
      new_rects.emplace_back(rect.x_, rect.y_ + item_height, item_width,
                             remaining_height);
    }
  } else {
    // Horizontal cut first
    if (remaining_height > 0) {
      new_rects.emplace_back(rect.x_, rect.y_ + item_height, rect.width_,
                             remaining_height);
    }
    if (remaining_width > 0) {
      new_rects.emplace_back(rect.x_ + item_width, rect.y_, remaining_width,
                             item_height);
    }
  }
  return new_rects;
}
