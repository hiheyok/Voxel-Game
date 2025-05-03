// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once

#include "UI/DisplayVertices.h"

class Display {
 public:
  Display() noexcept = default;

  Display(const Display&) noexcept = default;
  Display(Display&&) noexcept = default;

  Display& operator=(const Display&) noexcept = default;
  Display& operator=(Display&&) noexcept = default;

  DisplayVertices GetDisplayVertices();

 private:
  DisplayVertices render_info_;
};
