#include "LightEngine.h"
#include "../../Utils/Containers/FIFOQueue.h"

using namespace glm;

thread_local FixedFIFOQueue<uint16_t> FIFOQueues;

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
	if (curr < lvl) {
		container->EditLight(x, y, z, lvl);
	}
}

void LightingEngine::LightSpreadSky(Chunk* chunk, std::shared_ptr<ChunkLightingContainer>& container, const Heightmap& heightmap, int ChunkHeight, int x, int y, int z) {
	FIFOQueues.resetData();

	FIFOQueues.push(PackLightNode(x, y, z, 15));

	if (container->GetLighting(x, y, z) >= 15) {
		return;
	}


	int i = 0;

	while (!FIFOQueues.isEmpty()) {
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

		if (chunk->GetBlockUnsafe(nodeX, nodeY, nodeZ) != Blocks.AIR) {
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

std::vector<std::shared_ptr<ChunkLightingContainer>> LightingEngine::SkyLighting(const ChunkPos& pos) {
	uint8_t DarknessLightLevel = 12;
	std::vector<std::shared_ptr<ChunkLightingContainer>> out;
	ChunkColumn* col = world_->GetColumn(pos);

	if (col == nullptr) return out;

	const Heightmap& heightmap = world_->GetColumnHeightmap(pos);

	int relativeChunkHeight = pos.y & 0b11111;
	int columnYLevel = pos.y / 32;

	for (int i = 0; i <= relativeChunkHeight; i++) {
		if (!col->light_dirty_[i]) {
			continue;
		}

		std::shared_ptr<ChunkLightingContainer> lighting = std::make_shared<ChunkLightingContainer>();
		lighting->ResetLightingCustom(4);
		lighting->position_.set(pos.x, columnYLevel + i, pos.z);

		WorkOnChunkSkylight(col->GetChunk(i), lighting, heightmap, i * 16);

		out.push_back(std::move(lighting));
	}

	return out;
}

void LightingEngine::Generate(std::vector<ChunkPos> IDs) {
	std::lock_guard<std::mutex> lock{ scheduler_lock_ };
	task_queue_.insert(task_queue_.end(), IDs.begin(), IDs.end());
}

std::vector<std::shared_ptr<ChunkLightingContainer>> LightingEngine::GetOutput() {
	std::lock_guard<std::mutex> lock{ scheduler_lock_ };
	std::vector<std::shared_ptr<ChunkLightingContainer>> out = std::move(output_);
	output_.clear();

	return out;
}

void LightingEngine::Stop() {
	stop_ = true;
	scheduler_thread_.join();
	for (size_t i = 0; i < workers_.size(); i++) {
		workers_[i].join();
	}
}

void LightingEngine::Start(int lightEngineThreadsCount, WorldAccess* w) {
	stop_ = false;
	thread_count_ = lightEngineThreadsCount;
	world_ = w;

	workers_.resize(thread_count_);
	worker_task_.resize(thread_count_);
	worker_locks_.resize(thread_count_);
	worker_output_.resize(thread_count_);

	scheduler_thread_ = std::thread(&LightingEngine::Scheduler, this);

	for (int i = 0; i < thread_count_; i++) {
		workers_[i] = std::thread(&LightingEngine::Worker, this, i);
	}
}

void LightingEngine::QueueChunk(const ChunkColumnPos& columnID) {
	//queue chunk
	scheduler_lock_.lock();
	task_queue_.emplace_back(columnID);
	scheduler_lock_.unlock();
}

void LightingEngine::Worker(int id) {
	const int WorkerID = id;

	std::deque<ChunkColumnPos> jobs;
	std::deque<std::shared_ptr<ChunkLightingContainer>> finishedJobs;

	FIFOQueues.setSize(DEFAULT_FIFO_QUEUE_SIZE);

	while (!stop_) {
		//Fetches all of the tasks and put it in "Jobs"

		{
			std::lock_guard<std::mutex> lock{ worker_locks_[WorkerID] };
			if (!worker_task_[WorkerID].empty()) {
				jobs.insert(jobs.end(), worker_task_[WorkerID].begin(), worker_task_[WorkerID].end());
				worker_task_[WorkerID].clear();
			}
			
		}
		

		//Generates the chunks

		const size_t numJobs = jobs.size();
		const size_t batchSize = 100;

		for (size_t i = 0; i < std::min(numJobs, batchSize); i++) {
			ChunkPos task = std::move(jobs.front()); //fetches task
			jobs.pop_front();
			//Generate
			
			std::vector<std::shared_ptr<ChunkLightingContainer>> chunks = SkyLighting(task);
			finishedJobs.insert(finishedJobs.end(), std::make_move_iterator(chunks.begin()), std::make_move_iterator(chunks.end()));
		}

		if (finishedJobs.size() != 0) {
			std::lock_guard<std::mutex> lock{ worker_locks_[WorkerID] };
			worker_output_[WorkerID].insert(worker_output_[WorkerID].end(), std::make_move_iterator(finishedJobs.begin()), std::make_move_iterator(finishedJobs.end()));
			finishedJobs.clear();
		}
		timerSleepNotPrecise(1);
	}
	FIFOQueues.clear();

	jobs.clear();
	Logger.LogInfo("Light Engine", "Shutting down light engine worker: " + std::to_string(WorkerID));
}

void LightingEngine::Scheduler() {
	int workerSelection = 0;

	std::deque<std::deque<ChunkPos>> distributedTasks;
	std::deque<std::deque<std::shared_ptr<ChunkLightingContainer>>> chunkOutputs;

	distributedTasks.resize(thread_count_);
	chunkOutputs.resize(thread_count_);

	std::deque<ChunkPos> internalTaskList;

	while (!stop_) {

		// Fetch tasks
		{
			std::lock_guard<std::mutex> lock{ scheduler_lock_ };
			internalTaskList = std::move(task_queue_);
			task_queue_.clear();
		}
		

		// Internally distributes the jobs

		while (!internalTaskList.empty()) {

			ChunkPos task = std::move(internalTaskList.front());
			internalTaskList.pop_front();

			distributedTasks[workerSelection].push_back(task);

			workerSelection++;

			if (workerSelection == thread_count_)
				workerSelection = 0;
		}

		// Distributes the tasks

		for (int i = 0; i < thread_count_; i++) {
			std::lock_guard<std::mutex> lock{ worker_locks_[i] };
			worker_task_[i].insert(worker_task_[i].end(), std::make_move_iterator(distributedTasks[i].begin()), std::make_move_iterator(distributedTasks[i].end()));
			distributedTasks[i].clear();
		}

		//Fetches worker output
		for (int i = 0; i < thread_count_; i++) {
			std::lock_guard<std::mutex> lock{ worker_locks_[i] };
			chunkOutputs[i].insert(chunkOutputs[i].end(), std::make_move_iterator(worker_output_[i].begin()), std::make_move_iterator(worker_output_[i].end()));
			worker_output_[i].clear();
		}

		//Output the chunks so it can be used
		{
			std::lock_guard<std::mutex> lock{ scheduler_lock_ };
			for (int i = 0; i < thread_count_; i++) {
				output_.insert(output_.end(), std::make_move_iterator(chunkOutputs[i].begin()), std::make_move_iterator(chunkOutputs[i].end()));
				chunkOutputs[i].clear();
			}
		}
		

		timerSleepNotPrecise(1);

	}

	Logger.LogInfo("Light Engine", "Shutting down light engine scheduler");

}