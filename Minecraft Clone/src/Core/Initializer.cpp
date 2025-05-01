#include "Initializer.h"

#include "Core/Options/Option.h"
#include "Level/Block/Blocks.h"
#include "Level/Entity/Entities.h"
#include "Level/Entity/Entity.h"
#include "Level/Item/ItemTextureAtlas.h"
#include "Level/Item/Items.h"
#include "Level/TerrainGeneration/Biome/BiomeProvider.h"
#include "Level/TerrainGeneration/Generators/GeneratorType.h"
#include "Utils/LogUtils.h"

LogUtils g_logger;
Options g_app_options;
BlockList g_blocks;
EntitiesList g_entity_list;
ItemList g_items;
ItemTextureAtlas g_item_atlas;
GeneratorType g_generators;

int AmountEntityDestroy = 0;

void Initialize() {}
