// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include <cstdint>
#include <cstdlib>
#include <exception>

#include "Client/Client.h"
#define STB_IMAGE_IMPLEMENTATION
#include <time.h>

#include <iostream>

#include "Core/GameContext/GameContext.h"
#include "Core/Options/Option.h"
#include "Utils/stb_image.h"

int main() {
  stbi_set_flip_vertically_on_load(true);
  srand(static_cast<uint32_t>(time(0)));  // Set rng seed

  try {
    GameContext context;
    context.InitializeGameContext();
    Client GameClient{context};
    GameClient.run();
  } catch (std::exception& e) {
    std::cerr << e.what() << '\n';
    return -1;
  }

  return 0;
}
