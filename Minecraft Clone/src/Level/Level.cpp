#include "Level.h"
#include "Dimension/Dimensions/Overworld.h"
using namespace std;

void Level::Start(int worldGenThreadCount, int lightEngineThreadCount) {
	DimensionProperties properties;
	mainWorld = new OverworldDimension();
	mainWorld->Initialize(properties);
	
	long long Seed = -501575345763903LL;//-501575345763903LL;//-1587754402LL

	levelLoader.Start(worldGenThreadCount, lightEngineThreadCount, mainWorld->worldInteractions.worldLoader->getWorld(), Seed);
	Block::DimensionPTR = mainWorld;
}

void Level::Stop() {
	levelLoader.Stop();
}

void Level::updateDimensions() {
	//Generated Chunks
	levelLoader.sendRequestedChunks(mainWorld->worldInteractions.worldLoader->getRequestedChunks(), mainWorld->getGeneratorType());
	vector<Chunk*> chunks = levelLoader.getGeneratedChunk();
	mainWorld->worldInteractions.addChunks(chunks);

	//Updated Light Info
	levelLoader.sendRequestedLightUpdates(mainWorld->worldInteractions.getRequestedLightUpdates());
	vector<ChunkLightingContainer*> lighting = levelLoader.getLightingInfomation();
	mainWorld->worldInteractions.updateLighting(lighting);
}