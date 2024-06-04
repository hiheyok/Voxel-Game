#include "../Level/Chunk/Block/Blocks.h"
#include "../Level/Entity/Entities.h"
#include "../Level/Item/Items.h"
#include "../Utils/LogUtils.h"
#include "Initializer.h"
#include "../Level/Item/ItemTextureAtlas.h"
#include "../Level/Entity/Entity.h"
#include "../Level/TerrainGeneration/Generators/GeneratorType.h"
#include "Options/Option.h"

LogUtils Logger;
Options AppOptions;
BlockList Blocks;
EntitiesList EntityList;
ItemList Items;
ItemTextureAtlas ItemAtlas;
GeneratorType Generators;

int AmountEntityDestroy = 0;

void Initialize() {
	
}