// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#include "Level/Block/Type/Fluid.h"

#include "Core/GameContext/GameContext.h"
#include "FileManager/Files.h"
#include "Level/Block/Blocks.h"
#include "Level/Dimension/Dimension.h"
#include "Level/Event/EventHandler.h"
#include "RenderEngine/ChunkRender/BlockTextureAtlas.h"
#include "RenderEngine/Models/Block/BlockModelLoader.h"

Fluid::Fluid(GameContext& context, int spread_rate) : Block{context} {
  properties_->is_solid_ = false;
  properties_->is_fluid_ = true;
  properties_->light_pass_ = false;

  fluid_properties_.spread_rate_ = spread_rate;
}

std::unique_ptr<BlockModel> Fluid::InitializeBlockModel(
    BlockModelLoader& modelLoader) {
  (void)modelLoader;
  auto tokens = Tokenize(block_name_, ':');
  std::unique_ptr<BlockModel> block_model = std::make_unique<BlockModel>();

  BlockFace face;
  face.reference_texture_ =
      "block/" + tokens.back() + "_still:" + tokens.front();
  face.uv_ = {0, 0, 16, 16};
  BlockFace side;
  side.reference_texture_ =
      "block/" + tokens.back() + "_flow:" + tokens.front();
  side.uv_ = {0, 0, 16, 16};

  Cuboid cuboid;
  face.cull_face_ = Directions<BlockPos>::kUp;
  cuboid.EditFace(Directions<BlockPos>::kUp, face);

  side.cull_face_ = Directions<BlockPos>::kEast;
  cuboid.EditFace(Directions<BlockPos>::kEast, side);
  side.cull_face_ = Directions<BlockPos>::kWest;
  cuboid.EditFace(Directions<BlockPos>::kWest, side);
  side.cull_face_ = Directions<BlockPos>::kNorth;
  cuboid.EditFace(Directions<BlockPos>::kNorth, side);
  side.cull_face_ = Directions<BlockPos>::kSouth;
  cuboid.EditFace(Directions<BlockPos>::kSouth, side);
  side.cull_face_ = Directions<BlockPos>::kDown;
  cuboid.EditFace(Directions<BlockPos>::kDown, side);

  cuboid.to_.y = 15.f / 16;

  block_model->AddElement(cuboid);
  block_model->is_initialized_ = true;
  block_model->BakeTextureRotation();
  return block_model;
}

void Fluid::Tick(BlockPos pos, Dimension* currentWorld) {
  BlockID blockType = currentWorld->world_->GetBlock(pos);

  for (auto offset : Directions<BlockPos>()) {
    BlockPos newPos = pos + offset;

    if (offset == Directions<BlockPos>::kUp) {
      continue;
    }

    BlockID block = currentWorld->world_->GetBlock(newPos);

    if (block != context_.blocks_->AIR) {
      continue;
    }

    if (block == context_.blocks_->WATER) {
      continue;
    }

    if (currentWorld->CheckTickUsed(context_.event_handler_->BlockPlace, pos)) {
      return;
    }

    currentWorld->TickUsed(context_.event_handler_->BlockPlace, pos);
    BlockEvent blockEvent{newPos, blockType,
                          context_.event_handler_->BlockPlace};
    currentWorld->event_manager_.AddEvent(blockEvent);
  }
}
