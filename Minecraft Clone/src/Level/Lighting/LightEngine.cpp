#include "LightEngine.h"

using namespace std;
using namespace glm;

void LightingEngine::increaseLightLevel(ChunkLightingContainer* container, uint8_t lvl, int x, int y, int z) {
	uint8_t curr = container->GetLighting(x, y, z);
	if (curr < lvl) {
		container->EditLight(x, y, z, lvl);
	}
}

void LightingEngine::LightSpreadSky(Chunk* chunk, ChunkLightingContainer* container, vector<uint16_t>& heightmap, int ChunkHeight, int x, int y, int z) {
	deque<ivec4> BFS;

	BFS.push_back(ivec4(x, y, z, 15));

	if (container->GetLighting(x, y, z) >= 15) {
		return;
	}


	while (!BFS.empty()) {
		//Get node
		ivec4 node = BFS.front();
		BFS.pop_front();

		int nx = node.x;
		int ny = node.y + ChunkHeight;
		int nz = node.z;

		if (heightmap[nx * 16 + nz] <= ny) {
			node.w = 15;
		}

		if (container->GetLighting(node.x, node.y, node.z) >= node.w) {
			continue;
		}
		//Set node light level
		increaseLightLevel(container, node.w, node.x, node.y, node.z);

		if (chunk->GetBlockUnsafe(node.x, node.y, node.z) != Blocks.AIR) {
			continue;
		}

		if (node.w == 0) continue;

		//Spread
		for (int side = 0; side < 6; side++) {

			int direction = side >> 1;
			int face = side & 0b1; //1 = Front 0 = Back

			if ((direction == 1) && (face == 1))
				continue; //skip Up direction

			ivec4 newNode = node;
			newNode[direction] += face * 2 - 1;
			newNode.w -= 1;
			//Check if it is in the chunk first
			if (((newNode.x | newNode.y | newNode.z) >> 4))
				continue;

			//Check if the light level is more or less
			int currLvl = container->GetLighting(newNode.x, newNode.y, newNode.z);

			if (currLvl + 2 > node.w)
				continue;

			BFS.push_back(newNode);

		}
	}
}

void LightingEngine::WorkOnChunkSkylight(Chunk* chunk, ChunkLightingContainer* light, vector<uint16_t>& heightmap, int ChunkHeight) {
	for (int x = 0; x < 16; x++) {
		for (int z = 0; z < 16; z++) {

			int h = heightmap[x * 16 + z] - ChunkHeight; // it will try to find pivot points

			if (h >= 16) {
				continue;
			}

			LightSpreadSky(chunk, light, heightmap, ChunkHeight, x, 15, z);
		}
	}


}

vector<ChunkLightingContainer*> LightingEngine::SkyLighting(ChunkColumnID id) {
	uint8_t DarknessLightLevel = 12;
	vector<ChunkLightingContainer*> out = vector<ChunkLightingContainer*>(0);
	ivec3 pos = ChunkIDToPOS(id);
	ChunkColumn* col = world->getColumn(pos);

	if (col == nullptr) return out;
	Heightmap heightmap = world->getColumnHeightmap(pos);

	pos.y *= 32;

	

	for (int i = 0; i < 32; i++) {
		if (!col->LightDirty[i]) {
			continue;
		}

		ChunkLightingContainer* lighting = new ChunkLightingContainer;
		lighting->ResetLightingCustom(4);

		std::vector<uint16_t> h = heightmap.getData();
		WorkOnChunkSkylight(col->GetChunk(i), lighting, h, i * 16);

		lighting->Position = ivec3(pos.x, i + pos.y, pos.z);
		out.emplace_back(lighting);
	}


	return out;
}

void LightingEngine::Generate(vector<ChunkColumnID> IDs) {
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

void LightingEngine::queueChunk(ChunkColumnID columnID) {
	//queue chunk
	SchedulerLock.lock();
	TaskQueue.emplace_back(columnID);
	SchedulerLock.unlock();
}

void LightingEngine::worker(int id) {
	const int WorkerID = id;

	deque<ChunkColumnID> Jobs;

	deque<ChunkLightingContainer*> FinishedJobs;

	while (!stop) {
		//Fetches all of the tasks and put it in "Jobs"

		WorkerLocks[WorkerID].lock();
		Jobs.insert(Jobs.end(), WorkerTask[WorkerID].begin(), WorkerTask[WorkerID].end());
		WorkerTask[WorkerID].clear();
		WorkerLocks[WorkerID].unlock();

		//Generates the chunks

		const int NumJobs = Jobs.size();

		int count = 0;
		int BatchSize = 100;

		for (int i = 0; i < NumJobs; i++) {
			ChunkColumnID task = Jobs.front(); //fetches task
			Jobs.pop_front();
			//Generate
			ivec3 ColPos = ChunkIDToPOS(task);

			vector<ChunkLightingContainer*> chunks = SkyLighting(task);

			FinishedJobs.insert(FinishedJobs.end(), chunks.begin(), chunks.end());

			count++;
			if (count == BatchSize) {
				break;
			}
		}

		if (FinishedJobs.size() != 0) {
			WorkerLocks[WorkerID].lock();
			WorkerOutput[WorkerID].insert(WorkerOutput[WorkerID].end(), FinishedJobs.begin(), FinishedJobs.end());
			FinishedJobs.clear();
			WorkerLocks[WorkerID].unlock();
		}

		timerSleepNotPrecise(1);
	}

	Jobs.clear();
	Logger.LogInfo("Light Engine", "Shutting down light engine worker: " + to_string(WorkerID));
}

void LightingEngine::scheduler() {
	int WorkerSelection = 0;

	deque<deque<ChunkColumnID>> DistributedTasks;
	deque<deque<ChunkLightingContainer*>> ChunkOutputs;

	DistributedTasks.resize(threadCount);
	ChunkOutputs.resize(threadCount);

	deque<ChunkColumnID> InternalTaskList;

	while (!stop) {

		//Fetch tasks

		SchedulerLock.lock();
		InternalTaskList = TaskQueue;
		TaskQueue.clear();
		SchedulerLock.unlock();

		//Interally distributes the jobs

		while (!InternalTaskList.empty()) {

			ChunkColumnID task = InternalTaskList.front();
			InternalTaskList.pop_front();

			DistributedTasks[WorkerSelection].push_back(task);

			WorkerSelection++;

			if (WorkerSelection == threadCount)
				WorkerSelection = 0;
		}

		//Distributes the tasks

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