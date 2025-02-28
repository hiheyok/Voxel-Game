#include "LightEngine.h"
#include "../../Utils/Containers/FIFOQueue.h"
using namespace std;
using namespace glm;

thread_local FixedFIFOQueue<uint16_t> FIFOQueues;

void LightingEngine::increaseLightLevel(ChunkLightingContainer* container, uint8_t lvl, int x, int y, int z) {
	uint8_t curr = container->GetLighting(x, y, z);
	if (curr < lvl) {
		container->EditLight(x, y, z, lvl);
	}
}

inline uint16_t convertData(int x, int y, int z, int w) {
	return x | (y << 4) | (z << 8) | (w << 12);
}

void LightingEngine::LightSpreadSky(Chunk* chunk, ChunkLightingContainer* container, const Heightmap& heightmap, int ChunkHeight, int x, int y, int z) {
	FIFOQueues.resetData();

	FIFOQueues.push(convertData(x, y, z, 15));

	if (container->GetLighting(x, y, z) >= 15) {
		return;
	}


	int i = 0;

	while (!FIFOQueues.isEmpty()) {
		//Get node
		uint16_t node = FIFOQueues.get();
		uint8_t nodeX = node & 0b1111;
		uint8_t nodeY = (node >> 4) & 0b1111;
		uint8_t nodeZ = (node >> 8) & 0b1111;
		uint8_t nodeLight = (node >> 12) & 0b1111;

		i++;
		int nx = nodeX;
		int ny = nodeY + ChunkHeight;
		int nz = nodeZ;

		if (heightmap.get(nx, nz) <= ny) {
			nodeLight = 15;
		}

		if (container->GetLighting(nodeX, nodeY, nodeZ) >= nodeLight) {
			continue;
		}
		//Set node light level
		increaseLightLevel(container, nodeLight, nodeX, nodeY, nodeZ);

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

			uint8_t pos[3]{nodeX, nodeY, nodeZ};
			pos[direction] += face * 2 - 1;
			int8_t newLight = nodeLight - 1;
			//Check if it is in the chunk first
			if (((pos[0] | pos[1] | pos[2]) >> 4))
				continue;

			//Check if the light level is more or less
			int currLvl = container->GetLighting(pos[0], pos[1], pos[2]);

			if (currLvl + 2 > newLight)
				continue;

			FIFOQueues.push(convertData(pos[0], pos[1], pos[2], newLight));

		}
	}

	//std::cout << i << '\n';
}

void LightingEngine::WorkOnChunkSkylight(Chunk* chunk, ChunkLightingContainer* light, const Heightmap& heightmap, int ChunkHeight) {
	for (int x = 0; x < 16; x++) {
		for (int z = 0; z < 16; z++) {

			int h = heightmap.get(x, z) - ChunkHeight; // it will try to find pivot points

			if (h >= 16)
				continue;

			LightSpreadSky(chunk, light, heightmap, ChunkHeight, x, 15, z);
		}
	}


}

vector<ChunkLightingContainer*> LightingEngine::SkyLighting(const ChunkPos& pos, int workerID) {
	uint8_t DarknessLightLevel = 12;
	vector<ChunkLightingContainer*> out;
	ChunkColumn* col = world->getColumn(pos);

	if (col == nullptr) return out;

	const Heightmap& heightmap = world->getColumnHeightmap(pos);

	int relativeChunkHeight = pos.y & 0b11111;
	int columnYLevel = pos.y / 32;

	for (int i = 0; i <= relativeChunkHeight; i++) {
		if (!col->LightDirty[i]) {
			continue;
		}

		ChunkLightingContainer* lighting = new ChunkLightingContainer;
		lighting->ResetLightingCustom(4);
		lighting->position_.set(pos.x, columnYLevel + i, pos.z);

		WorkOnChunkSkylight(col->GetChunk(i), lighting, heightmap, i * 16);

		out.emplace_back(lighting);
	}

	return out;
}

void LightingEngine::Generate(vector<ChunkPos> IDs) {
	SchedulerLock.lock();
	TaskQueue.insert(TaskQueue.end(), IDs.begin(), IDs.end());
	SchedulerLock.unlock();
}

vector<ChunkLightingContainer*> LightingEngine::GetOutput() {
	SchedulerLock.lock();
	vector<ChunkLightingContainer*> out = Output;
	Output.clear();
	SchedulerLock.unlock();

	return out;
}

void LightingEngine::Stop() {
	stop = true;
	schedulerThread.join();
	for (size_t i = 0; i < Workers.size(); i++) {
		Workers[i].join();
	}
}

void LightingEngine::Start(int lightEngineThreadsCount, WorldAccess* w) {
	stop = false;
	threadCount = lightEngineThreadsCount;
	world = w;

	Workers.resize(threadCount);
	WorkerTask.resize(threadCount);
	WorkerLocks.resize(threadCount);
	WorkerOutput.resize(threadCount);

	schedulerThread = thread(&LightingEngine::scheduler, this);

	for (int i = 0; i < threadCount; i++) {
		Workers[i] = thread(&LightingEngine::worker, this, i);
	}
}

void LightingEngine::queueChunk(const ChunkColumnPos& columnID) {
	//queue chunk
	SchedulerLock.lock();
	TaskQueue.emplace_back(columnID);
	SchedulerLock.unlock();
}

void LightingEngine::worker(int id) {
	const int WorkerID = id;

	deque<ChunkColumnPos> Jobs;
	deque<ChunkLightingContainer*> FinishedJobs;

	FIFOQueues.setSize(DEFAULT_FIFO_QUEUE_SIZE);

	while (!stop) {
		//Fetches all of the tasks and put it in "Jobs"

		WorkerLocks[WorkerID].lock();
		Jobs.insert(Jobs.end(), WorkerTask[WorkerID].begin(), WorkerTask[WorkerID].end());
		WorkerTask[WorkerID].clear();
		WorkerLocks[WorkerID].unlock();

		//Generates the chunks

		const size_t numJobs = Jobs.size();

		int count = 0;
		const size_t batchSize = 100;

		for (size_t i = 0; i < std::min(numJobs, batchSize); i++) {
			ChunkPos task = Jobs.front(); //fetches task
			Jobs.pop_front();
			//Generate
			
			vector<ChunkLightingContainer*> chunks = SkyLighting(task, id);
			FinishedJobs.insert(FinishedJobs.end(), chunks.begin(), chunks.end());
		}

		if (FinishedJobs.size() != 0) {
			WorkerLocks[WorkerID].lock();
			WorkerOutput[WorkerID].insert(WorkerOutput[WorkerID].end(), FinishedJobs.begin(), FinishedJobs.end());
			FinishedJobs.clear();
			WorkerLocks[WorkerID].unlock();
		}
		timerSleepNotPrecise(1);
	}
	FIFOQueues.clear();

	Jobs.clear();
	Logger.LogInfo("Light Engine", "Shutting down light engine worker: " + to_string(WorkerID));
}

void LightingEngine::scheduler() {
	int WorkerSelection = 0;

	deque<deque<ChunkPos>> DistributedTasks;
	deque<deque<ChunkLightingContainer*>> ChunkOutputs;

	DistributedTasks.resize(threadCount);
	ChunkOutputs.resize(threadCount);

	deque<ChunkPos> InternalTaskList;

	while (!stop) {

		// Fetch tasks

		SchedulerLock.lock();
		InternalTaskList = TaskQueue;
		TaskQueue.clear();
		SchedulerLock.unlock();

		// Internally distributes the jobs

		while (!InternalTaskList.empty()) {

			ChunkPos task = InternalTaskList.front();
			InternalTaskList.pop_front();

			DistributedTasks[WorkerSelection].push_back(task);

			WorkerSelection++;

			if (WorkerSelection == threadCount)
				WorkerSelection = 0;
		}

		// Distributes the tasks

		for (int i = 0; i < threadCount; i++) {
			WorkerLocks[i].lock();
			WorkerTask[i].insert(WorkerTask[i].end(), DistributedTasks[i].begin(), DistributedTasks[i].end());
			WorkerLocks[i].unlock();
			DistributedTasks[i].clear();
		}

		//Fetches worker output
		for (int i = 0; i < threadCount; i++) {
			WorkerLocks[i].lock();
			ChunkOutputs[i].insert(ChunkOutputs[i].end(), WorkerOutput[i].begin(), WorkerOutput[i].end());
			WorkerOutput[i].clear();
			WorkerLocks[i].unlock();
		}

		//Output the chunks so it can be used

		SchedulerLock.lock();
		for (int i = 0; i < threadCount; i++) {
			Output.insert(Output.end(), ChunkOutputs[i].begin(), ChunkOutputs[i].end());
			ChunkOutputs[i].clear();
		}
		SchedulerLock.unlock();

		timerSleepNotPrecise(1);

	}

	Logger.LogInfo("Light Engine", "Shutting down light engine scheduler");

}