#include "../World/Chunk/Block/Blocks.h"
#include "../World/Entity/Entities.h"
#include "../World/Item/Items.h"
#include "../Utils/LogUtils.h"
#include "Initializer.h"
#include "../World/Item/ItemTextureAtlas.h"
#include "../World/Entity/Entity.h"
#include "../World/TerrainGeneration/Generators/GeneratorType.h"

LogUtils Logger;
BlockList Blocks;
EntitiesList EntityList;
ItemList Items;
ItemTextureAtlas ItemAtlas;
GeneratorType Generators;

int AmountEntityDestroy = 0;

void Initialize() {
	
}