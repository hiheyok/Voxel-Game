#include "Level.h"

using namespace std;

void Level::Start(int worldGenThreadCount, int lightEngineThreadCount) {
	DimensionProperties properties;
	mainWorld.Initialize(properties);
	levelLoader.Start(worldGenThreadCount, lightEngineThreadCount, mainWorld.worldInteractions.worldLoader->getWorld());
	Block::DimensionPTR = &mainWorld;
}

void Level::Stop() {
	levelLoader.Stop();
}

void Level::updateDimensions() {
	//Generated Chunks
	levelLoader.sendRequestedChunks(mainWorld.worldInteractions.worldLoader->getRequestedChunks());
	vector<Chunk*> chunks = levelLoader.getGeneratedChunk();
	mainWorld.worldInteractions.addChunks(chunks);

	//Updated Light Info
	levelLoader.sendRequestedLightUpdates(mainWorld.worldInteractions.getRequestedLightUpdates());
	vector<ChunkLightingContainer*> lighting = levelLoader.getLightingInfomation();
	mainWorld.worldInteractions.updateLighting(lighting);
}