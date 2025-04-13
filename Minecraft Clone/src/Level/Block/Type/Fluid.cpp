#include "Level/Block/Type/Fluid.h"
#include "Level/Dimension/Dimension.h"
#include "Level/Event/EventHandler.h"
#include "RenderEngine/BlockModel/ModelLoader.h"
#include "RenderEngine/ChunkRender/BlockTextureAtlas.h"
#include "FileManager/Files.h"

void Fluid::InitializeBlockModel(ModelLoader & modelLoader) {
    auto tokens = Tokenize(block_name_, ':');
    block_model_data_ = std::make_unique<BlockModel>();
    BlockFace face;
    face.reference_texture_ = "block/" + tokens.back() + "_still:" + tokens.front();

    BlockFace side;
    side.reference_texture_ = "block/" + tokens.back() + "_flow:" + tokens.front();

    Cuboid cuboid;
    face.cull_face_ = UP;
    cuboid.EditFace(UP, face);

    side.cull_face_ = EAST;
    cuboid.EditFace(EAST, side);
    side.cull_face_ = WEST;
    cuboid.EditFace(WEST, side);
    side.cull_face_ = NORTH;
    cuboid.EditFace(NORTH, side);
    side.cull_face_ = SOUTH;
    cuboid.EditFace(SOUTH, side);
    side.cull_face_ = DOWN;
    cuboid.EditFace(DOWN, side);
    block_model_data_->AddElement(cuboid);
    block_model_data_->is_initialized_ = true;
}

void Fluid::Tick(const BlockPos& pos, Dimension* currentWorld) {
    BlockID blockType = currentWorld->world_->GetBlock(pos);

    for (const auto& offset : Directions()) {
        BlockPos newPos = pos + offset;

        if (offset == Directions::kUp) { 
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
        BlockEvent blockEvent{ newPos, blockType, g_event_handler.BlockPlace };
        currentWorld->event_manager_.AddEvent(blockEvent);

    }
}