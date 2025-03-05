#include "LightEngine.h"
#include "../../Utils/Containers/FIFOQueue.h"
static thread_local FixedFIFOQueue<uint16_t> FIFOQueues;

constexpr int8_t DIRECTION_OFFSETS[6][3] = {
	{-1, 0, 0}, {1, 0, 0},  // X: Left, Right
	{0, -1, 0}, {0, 1, 0},  // Y: Down, Up
	{0, 0, -1}, {0, 0, 1}   // Z: Back, Front
};

// Pack light node data into uint16_t
inline uint16_t PackLightNode(uint8_t x, uint8_t y, uint8_t z, uint8_t light) {
	return x | (y << 4) | (z << 8) | (light << 12);
}

// Unpack light node data
inline void UnpackLightNode(uint16_t node, uint8_t& x, uint8_t& y, uint8_t& z, uint8_t& light) {
	x = node & 0xF;
	y = (node >> 4) & 0xF;
	z = (node >> 8) & 0xF;
	light = (node >> 12) & 0xF;
}

void LightingEngine::IncreaseLightLevel(std::shared_ptr<ChunkLightingContainer>& container, uint8_t lvl, int x, int y, int z) {
	uint8_t curr = container->GetLighting(x, y, z);
	if (curr < lvl)
		container->EditLight(x, y, z, lvl);
}

void LightingEngine::LightSpreadSky(Chunk* chunk, std::shared_ptr<ChunkLightingContainer>& container, const Heightmap& heightmap, int ChunkHeight, int x, int y, int z) {
	FIFOQueues.resetData();
	FIFOQueues.push(PackLightNode(x, y, z, 15));

	if (container->GetLighting(x, y, z) >= 15)
		return;

	int i = 0;

	while (!FIFOQueues.IsEmpty()) {
		//Get node
		uint16_t node = FIFOQueues.get();
		uint8_t nodeX, nodeY, nodeZ, nodeLight;

		UnpackLightNode(node, nodeX, nodeY, nodeZ, nodeLight);

		i++;
		int nx = nodeX;
		int ny = nodeY + ChunkHeight;
		int nz = nodeZ;

		if (heightmap.Get(nx, nz) <= ny) {
			nodeLight = 15;
		}

		if (container->GetLighting(nodeX, nodeY, nodeZ) >= nodeLight) {
			continue;
		}
		//Set node light level
		IncreaseLightLevel(container, nodeLight, nodeX, nodeY, nodeZ);

		if (chunk->GetBlockUnsafe(nodeX, nodeY, nodeZ) != g_blocks.AIR) {
			continue;
		}

		if (nodeLight == 0) continue;

		//Spread
		for (int side = 0; side < 6; side++) {

			int direction = side >> 1;
			int face = side & 0b1; //1 = Front 0 = Back

			if ((direction == 1) && (face == 1))
				continue; //skip Up direction

			uint8_t nx = nodeX + DIRECTION_OFFSETS[side][0];
			uint8_t ny = nodeY + DIRECTION_OFFSETS[side][1];
			uint8_t nz = nodeZ + DIRECTION_OFFSETS[side][2];

			int8_t newLight = nodeLight - 1;

			//Check if it is in the chunk first
			if (((nx | ny | nz) >> 4))
				continue;

			//Check if the light level is more or less
			int currLvl = container->GetLighting(nx, ny, nz);

			if (currLvl + 2 > newLight)
				continue;

			FIFOQueues.push(PackLightNode(nx, ny, nz, newLight));

		}
	}
}

void LightingEngine::WorkOnChunkSkylight(Chunk* chunk, std::shared_ptr<ChunkLightingContainer>& light, const Heightmap& heightmap, int chunkHeight) {
	for (int x = 0; x < 16; x++) {
		for (int z = 0; z < 16; z++) {
			int h = heightmap.Get(x, z) - chunkHeight; // it will try to find pivot points

			if (h >= 16)
				continue;

			LightSpreadSky(chunk, light, heightmap, chunkHeight, x, 15, z);
		}
	}
}

std::vector<std::shared_ptr<ChunkLightingContainer>> LightingEngine::Worker(const ChunkPos& pos) {
	if (!FIFOQueues.IsInitialized())
		FIFOQueues.setSize(LightingEngine::DEFAULT_FIFO_QUEUE_SIZE);

	uint8_t DarknessLightLevel = 12;
	std::vector<std::shared_ptr<ChunkLightingContainer>> out;
	ChunkColumn* col = LightingEngine::world_->GetColumn(pos);

	if (col == nullptr) return out;

	const Heightmap& heightmap = world_->GetColumnHeightmap(pos);

	int relativeChunkHeight = pos.y & 0b11111;
	int columnYLevel = pos.y / 32;

	for (int i = 0; i <= relativeChunkHeight; i++) {
		if (!col->light_dirty_[i])
			continue;

		std::shared_ptr<ChunkLightingContainer> lighting = std::make_shared<ChunkLightingContainer>();
		lighting->ResetLightingCustom(4);
		lighting->position_.set(pos.x, columnYLevel + i, pos.z);
		WorkOnChunkSkylight(col->GetChunk(i), lighting, heightmap, i * 16);

		out.push_back(std::move(lighting));
	}

	return out;
}

void LightingEngine::Generate(std::vector<ChunkPos> tasks) {
	lighting_thread_pool_->SubmitTask(tasks);
}

std::vector<std::shared_ptr<ChunkLightingContainer>> LightingEngine::GetOutput() {
	std::vector<std::shared_ptr<ChunkLightingContainer>> out;
	for (auto& lights : lighting_thread_pool_->GetOutput()) {
		out.insert(out.end(),
			std::make_move_iterator(lights.begin()),
			std::make_move_iterator(lights.end()));
	}
	return out;
}

void LightingEngine::Stop() {
	lighting_thread_pool_->Stop();
}

void LightingEngine::Start(int lightEngineThreadsCount, WorldAccess* w) {
	lighting_thread_pool_ = std::make_unique<ThreadPool<ChunkPos,
		std::vector<std::shared_ptr<ChunkLightingContainer>>,
		LightingEngine::Worker>>(lightEngineThreadsCount, "Light Engine", 100);
	LightingEngine::world_ = w;
}

void LightingEngine::QueueChunk(const ChunkPos& pos) {
	lighting_thread_pool_->SubmitTask(pos);
}