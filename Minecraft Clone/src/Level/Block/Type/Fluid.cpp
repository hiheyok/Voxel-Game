#include "Level/Block/Type/Fluid.h"

#include "FileManager/Files.h"
#include "Level/Dimension/Dimension.h"
#include "Level/Event/EventHandler.h"
#include "RenderEngine/BlockModel/ModelLoader.h"
#include "RenderEngine/ChunkRender/BlockTextureAtlas.h"

Fluid::Fluid(int spread_rate) {
    properties_->is_solid_ = false;
    properties_->is_fluid_ = true;
    properties_->light_pass_ = false;

    fluid_properties_.spread_rate_ = spread_rate;
}

void Fluid::InitializeBlockModel(ModelLoader& modelLoader) {
    auto tokens = Tokenize(block_name_, ':');
    block_model_data_ = std::make_unique<BlockModel>();
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

    cuboid.to_.y = 15;

    block_model_data_->AddElement(cuboid);
    block_model_data_->is_initialized_ = true;
    block_model_data_->BakeTextureRotation();
}

void Fluid::Tick(const BlockPos& pos, Dimension* currentWorld) {
    BlockID blockType = currentWorld->world_->GetBlock(pos);

    for (const auto& offset : Directions<BlockPos>()) {
        BlockPos newPos = pos + offset;

        if (offset == Directions<BlockPos>::kUp) {
            continue;
        }

        BlockID block = currentWorld->world_->GetBlock(newPos);

        if (block != g_blocks.AIR) {
            continue;
        }

        if (block == g_blocks.WATER) {
            continue;
        }

        if (currentWorld->CheckTickUsed(g_event_handler.BlockPlace, pos)) {
            return;
        }

        currentWorld->TickUsed(g_event_handler.BlockPlace, pos);
        BlockEvent blockEvent{newPos, blockType, g_event_handler.BlockPlace};
        currentWorld->event_manager_.AddEvent(blockEvent);
    }
}