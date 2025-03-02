#include "Level.h"
#include "Dimension/Dimensions/Overworld.h"
using namespace std;

void Level::Start(int worldGenThreadCount, int light_engine_thread_count_) {
	DimensionProperties properties;
	main_world_ = new OverworldDimension();
	main_world_->Initialize(properties);
	
	long long Seed = -501575345763903LL;//-501575345763903LL;//-1587754402LL

	level_loader_.Start(worldGenThreadCount, light_engine_thread_count_, main_world_->world_interactions_.worldLoader_->GetWorld(), Seed);
	Block::dimension_ptr_ = main_world_;
}

void Level::Stop() {
	level_loader_.Stop();
}

void Level::updateDimensions() {
	//Generated Chunks
	level_loader_.SendRequestedChunks(main_world_->world_interactions_.worldLoader_->GetRequestedChunks(), main_world_->GetGeneratorType());
	vector<Chunk*> chunks = level_loader_.GetGeneratedChunk();
	main_world_->world_interactions_.AddChunks(chunks);

	//Updated Light Info
	level_loader_.SendRequestedLightUpdates(main_world_->world_interactions_.GetRequestedLightUpdates());
	vector<std::shared_ptr<ChunkLightingContainer>> lighting = level_loader_.GetLightingInfomation();
	main_world_->world_interactions_.UpdateLighting(std::move(lighting));
}