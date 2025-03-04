#include "Generator.h"
#include "../Chunk/ChunkPos/ChunkPos.h"
#include "../../Utils/Clock.h"
#include "Generators/GeneratorType.h"
#include <concurrent_unordered_set.h>

using namespace std;
using namespace glm;

vector<Chunk*> ChunkGeneration::GetOutput() {
	std::vector<Chunk*> output;
	for (const auto& chunks : gen_pool_->GetOutput()) {
		output.insert(output.end(), std::make_move_iterator(chunks.begin()), std::make_move_iterator(chunks.end()));
	}

	return output;
}

void ChunkGeneration::Start(int threadCount, long long int worldSeedIn) {
	gen_pool_ = std::make_unique<ThreadPool<std::pair<ChunkPos, WorldGeneratorID>, std::vector<Chunk*>, ChunkGeneration::Worker>>(threadCount, "World Generator", 100);
	WorldGenerator::SetSeed(worldSeedIn);
}

void ChunkGeneration::Stop() {
	gen_pool_->Stop();
}

/*
* Static function for the thread pool to use
*/
std::vector<Chunk*> ChunkGeneration::Worker(const std::pair<ChunkPos, WorldGeneratorID>& task ) {
	//Generate
	const ChunkPos& pos = task.first;
	const WorldGeneratorID& generatorUse = task.second;
	std::vector<Chunk*> output;

	if (!g_generators.GetGenerator(generatorUse)->use_tall_chunks_) {
		Chunk* chunk = g_generators.GetGenerator(generatorUse)->Generate(pos);

		chunk->position_ = pos;
		output.push_back(chunk);
	}
	else {
		TallChunk* chunk = nullptr;
		if (pos.y != 0) {
			chunk = new TallChunk();
		}
		else {
			chunk = g_generators.GetGenerator(generatorUse)->GenerateTall(pos);
		}
		output = chunk->GetCubicChunks();
	}

	return output;
}


void ChunkGeneration::Generate(ChunkPos pos, WorldGeneratorID genType) {
	gen_pool_->SubmitTask(std::pair<ChunkPos, WorldGeneratorID>(pos, genType));
}

void ChunkGeneration::Generate(vector<ChunkPos> ids, WorldGeneratorID genTypeIn) {
	std::vector<std::pair<ChunkPos, WorldGeneratorID>> tasks;
	for (const auto& chunkId : ids) {
		tasks.emplace_back(chunkId, genTypeIn);
	}
	gen_pool_->SubmitTask(tasks);
}
