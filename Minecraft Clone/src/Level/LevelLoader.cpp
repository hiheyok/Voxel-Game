#include "LevelLoader.h"

using namespace std;

void LevelLoader::Start(int worldGenThreadCount, int lightEngineThreadCount, WorldAccess* world) {
	worldGenerator.Start(worldGenThreadCount);
	lightEngine.Start(lightEngineThreadCount, world);
}

void LevelLoader::Stop() {
	worldGenerator.Stop();
}

int LevelLoader::getChunkCount() {
	return count;
}

void LevelLoader::sendRequestedChunks(vector<ChunkID> RequestedChunks) { //Add option for requested chunks dimension type and generator type later
	//Later this will check on the disk for chunks
	worldGenerator.Generate(RequestedChunks);
}

vector<Chunk*> LevelLoader::getGeneratedChunk() {
	vector<Chunk*> out = worldGenerator.GetOutput();
	count += out.size();
	return out;
}

void LevelLoader::sendRequestedLightUpdates(vector<ChunkColumnID> RequestedLight) {
	//Later this will check on the disk for baked light info
	lightEngine.Generate(RequestedLight);
}

vector<ChunkLightingContainer*> LevelLoader::getLightingInfomation() {
	vector<ChunkLightingContainer*> out = lightEngine.GetOutput();
	return out;
}