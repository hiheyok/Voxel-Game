#include "WorldRender.h"
#include "../../Utils/Clock.h"

using namespace std;
using namespace glm;

using namespace Meshing;

void WorldRender::Render() {
	RendererV2.RenderSky();
	RendererV2.Render();
}

void WorldRender::LoadChunkToRenderer(ChunkID chunk) {
	if (!world->GetChunk(chunk).isEmpty) {
		SchedulerLock.lock();
		TaskList.push_back(chunk);
		SchedulerLock.unlock();
		
	}
	
}


void WorldRender::Worker(int id) {

	const int WorkerID = id;

	deque<ChunkID> Jobs;

	deque<ChunkMeshData> FinishedJobs;

	while (!stop) {
		//Fetches all of the tasks and put it in "Jobs"

		WorkerLocks[WorkerID].lock();
		Jobs.insert(Jobs.end(), WorkerTask[WorkerID].begin(), WorkerTask[WorkerID].end());
		WorkerTask[WorkerID].clear();
		WorkerLocks[WorkerID].unlock();
		
		int count = 0;

		int BatchSize = 500;

		while (!Jobs.empty()) {
			ChunkID task = Jobs.front(); //fetches task
			Jobs.pop_front();
			ivec3 pos = ChunkIDToPOS(task);

			//Generates the meshes
			auto t0 = std::chrono::high_resolution_clock::now();

			FinishedJobs.emplace_back(world->GetChunk(pos.x, pos.y, pos.z));
			
			auto t1 = std::chrono::high_resolution_clock::now();

			count++;

			if ((count % BatchSize) == 0) {
				break;
			}
		}
		
		if (FinishedJobs.size() != 0) {
			WorkerLocks[WorkerID].lock();
			WorkerOutput[WorkerID].insert(WorkerOutput[WorkerID].end(), FinishedJobs.begin(), FinishedJobs.end());
			FinishedJobs.clear();
			WorkerLocks[WorkerID].unlock();
		}
		

		timerSleepNotPrecise(3);
	}

	getLogger()->LogInfo("Mesher", "Shutting down mesh worker: " + std::to_string(WorkerID));
}

void WorldRender::Update() {

	for (int WorkerID = 0; WorkerID < WorkerCount; WorkerID++) {
		
		WorkerLocks[WorkerID].lock();

		while (!WorkerOutput[WorkerID].empty()) {


			RendererV2.AddChunk(WorkerOutput[(uint64_t)WorkerID].front());
			RendererV2.Defrag(1);
			WorkerOutput[WorkerID].pop_front();
		}

		WorkerLocks[WorkerID].unlock();
	}

	std::unordered_set<ChunkID> ChunksToUpdate;

	world->ChunkUpdateLock.lock();
	for (ChunkID task : world->ChunksUpdated) {
		ChunksToUpdate.insert(task);
	}
	world->ChunksUpdated.clear();
	world->ChunkUpdateLock.unlock();

	for (ChunkID chunkid : ChunksToUpdate) {
		LoadChunkToRenderer(chunkid);
	}
	
	RendererV2.Defrag(25);
	RendererV2.Update();
	RendererV2.PrepareRenderer();

}

void WorldRender::Stop() {
	stop = true;
	Scheduler.join();
	for (int i = 0; i < Workers.size(); i++) {
		Workers[i].join();
	}

	RendererV2.Cleanup();
	
}

void WorldRender::Start(GLFWwindow* window_,World* world_, int ThreadCount) {
	stop = false;

	window = window_;
	world = world_;
	WorkerCount = ThreadCount;
	
	RendererV2.Initialize(window, player.getCamera());
	RendererV2.LoadAssets();
	RendererV2.setSettings(renderDistance, 90);
	
	WorkerCount = ThreadCount;

	Workers.resize(ThreadCount);
	WorkerTask.resize(ThreadCount);
	WorkerOutput.resize(ThreadCount);
	WorkerLocks.resize(ThreadCount);

	for (int i = 0; i < ThreadCount; i++) {
		Workers[i] = std::thread(&WorldRender::Worker, this, i);
	}
	Scheduler = std::thread(&WorldRender::TaskScheduler, this);
}

void WorldRender::TaskScheduler() {
	
	int WorkerSelection = 0;

	std::deque<std::deque<ChunkID>> DistributedTasks;

	DistributedTasks.resize(WorkerCount);

	std::deque<ChunkID> InternalTaskList;

	while (!stop) {

		//Fetch tasks

		SchedulerLock.lock();
		InternalTaskList.insert(InternalTaskList.end(), TaskList.begin(), TaskList.end());
		TaskList.clear();
		SchedulerLock.unlock();

		//Interally distributes the jobs

		while (!InternalTaskList.empty()) {

			ChunkID task = InternalTaskList.front();
			InternalTaskList.pop_front();

			DistributedTasks[WorkerSelection].push_back(task);

			WorkerSelection++;

			if (WorkerSelection % WorkerCount == 0)
				WorkerSelection = 0;
		}

		//Distributes the tasks

		for (int i = 0; i < WorkerCount; i++) {
			WorkerLocks[i].lock();
			WorkerTask[i].insert(WorkerTask[i].end(), DistributedTasks[i].begin(), DistributedTasks[i].end());
			WorkerLocks[i].unlock();

			DistributedTasks[i].clear();
		}

		timerSleepNotPrecise(5);

	}

	getLogger()->LogInfo("Mesher", "Shutting down mesh scheduler");
	
}