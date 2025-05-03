// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "Client/Client.h"
#include "Level/Block/Blocks.h"
#include "Utils/LogUtils.h"
#define STB_IMAGE_IMPLEMENTATION
#include <time.h>

#include <cstdlib>

#include "Core/Initializer.h"
#include "Core/Options/Option.h"
#include "Utils/stb_image.h"

#include <iostream>

int main() {
  stbi_set_flip_vertically_on_load(true);
  srand(static_cast<uint32_t>(time(0)));  // Set rng seed

  Initialize();

  Client GameClient;
  GameClient.run();

  Options options;
  return 0;
}
