#include "../World/Chunk/Block/Blocks.h"
#include "../World/Entity/Entities.h"
#include "../World/Item/Items.h"
#include "../Utils/LogUtils.h"
#include "Initializer.h"
#include "../World/Item/ItemTextureAtlas.h"
#include "../World/Entity/Entity.h"

LogUtils Logger;
BlockList Blocks;
EntitiesList EntityList;
ItemList Items;
ItemTextureAtlas ItemAtlas;

int AmountEntityDestroy = 0;

void Initialize() {
	
}