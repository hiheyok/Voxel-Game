#include "WorldRender.h"
#include "../../Utils/Clock.h"

using namespace std;
using namespace glm;

using namespace Meshing;

#define RENDERV2w true

void WorldRender::Render() {
#ifndef RENDERV2
	RendererV2.RenderSky();
	RendererV2.Render();
#else
	Renderer.draw();
#endif
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
		
		while (!Jobs.empty()) {
			ChunkID task = Jobs.front(); //fetches task
			Jobs.pop_front();
			ivec3 pos = ChunkIDToPOS(task);

			//Generates the meshes
			auto t0 = std::chrono::high_resolution_clock::now();
			ChunkMeshData MeshData(world->GetChunk(pos.x, pos.y, pos.z));
			if (MeshData.SolidVertices.size() + MeshData.TransparentVertices.size() != 0) {
				FinishedJobs.emplace_back(MeshData);
			}
			
			auto t1 = std::chrono::high_resolution_clock::now();
		}

		WorkerLocks[WorkerID].lock();
		WorkerOutput[WorkerID].insert(WorkerOutput[WorkerID].end(), FinishedJobs.begin(), FinishedJobs.end());
		FinishedJobs.clear();
		WorkerLocks[WorkerID].unlock();

		timerSleepNotPrecise(5);
	}

	getLogger()->LogInfo("Mesher", "Shutting down mesh worker: " + std::to_string(WorkerID));
}

void WorldRender::Update() {

	//int ChunksPerFrame = 250;
	//int Count = 0;

	for (int WorkerID = 0; WorkerID < WorkerCount; WorkerID++) {
		
		WorkerLocks[WorkerID].lock();

		if (WorkerOutput[WorkerID].size() != 0) {
			getLogger()->LogInfo("Rendering", "Adding Chunks: " + std::to_string(WorkerOutput[WorkerID].size()));
		}
		

		while (!WorkerOutput[WorkerID].empty()) {


			if (WorkerOutput[WorkerID].front().SolidVertices.size() != 0) {
#ifndef RENDERV2
				RendererV2.AddChunk(WorkerOutput[(uint64_t)WorkerID].front());
#else
				Renderer.AddChunkMesh(WorkerOutput[(uint64_t)WorkerID].front());
#endif
			}
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
	
#ifndef RENDERV2
	RendererV2.Update();
	RendererV2.PrepareRenderer();
#else
	Renderer.UpdateData();
	Renderer.GenCallDrawCommands();
#endif
	

}

void WorldRender::Stop() {
	stop = true;
	Scheduler.join();
	for (int i = 0; i < Workers.size(); i++) {
		Workers[i].join();
	}

#ifndef RENDERV2
	RendererV2.Cleanup();
#else
	Renderer.Cleanup();
#endif
	
}

void WorldRender::Start(GLFWwindow* window_,World* world_, int ThreadCount) {
	stop = false;

	window = window_;
	world = world_;
	WorkerCount = ThreadCount;
	
#ifndef RENDERV2
	RendererV2.Initialize(window, player.getCamera());
	RendererV2.LoadAssets();
	RendererV2.setSettings(renderDistance, 90);
#else
	Renderer.init(window, player.getCamera());
	Renderer.ReloadAssets();
	Renderer.RenderDistance = renderDistance;
#endif
	
	
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