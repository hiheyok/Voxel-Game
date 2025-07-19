// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "Client/Client.h"
#define STB_IMAGE_IMPLEMENTATION
#include <time.h>

#include <iostream>
#include <stdexcept>

#include "Core/GameContext/GameContext.h"
#include "Core/Options/Option.h"
#include "Utils/stb_image.h"

int main() {
  stbi_set_flip_vertically_on_load(true);
  srand(static_cast<uint32_t>(time(0)));  // Set rng seed

  GameContext game_context;
  game_context.InitializeGameContext();
  Client GameClient{game_context};
  GameClient.run();
  return 0;
}
