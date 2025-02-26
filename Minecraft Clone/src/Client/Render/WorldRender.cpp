#include "WorldRender.h"
#include "../../Utils/Clock.h"
#include "../../Core/Options/Option.h"

using namespace std;
using namespace glm;
using namespace MeshingV2;

void WorldRender::SetRotation(dvec2 rotation) {
	player.SetRotation(rotation);
}

void WorldRender::SetPosition(dvec3 position) {
	player.SetPosition(position);
}

void WorldRender::Render() {
	RendererV2.RenderSky();
	RendererV2.Render();
}

void WorldRender::LoadChunkToRenderer(ChunkPos chunk) {
	if (server->getChunk(chunk) != nullptr) {
		SchedulerLock.lock();
		TaskList.push_back(chunk);
		SchedulerLock.unlock();
	}

}

void WorldRender::LoadChunkMultiToRenderer(std::vector<ChunkPos> chunks) {
	SchedulerLock.lock();
	TaskList.insert(TaskList.end(), chunks.begin(), chunks.end());
	SchedulerLock.unlock();
}


void WorldRender::Worker(int id) {

	const int WorkerID = id;

	deque<ChunkPos> Jobs;
	ChunkMeshData ChunkMesher;

	deque<ChunkVertexData*> FinishedJobs;

	while (!stop) {
		//Fetches all of the tasks and put it in "Jobs"

		WorkerLocks[WorkerID].lock();
		Jobs.insert(Jobs.end(), WorkerTask[WorkerID].begin(), WorkerTask[WorkerID].end());
		WorkerTask[WorkerID].clear();
		WorkerLocks[WorkerID].unlock();

		int count = 0;

		int BatchSize = 500;

		while (!Jobs.empty()) {
			ChunkPos pos = Jobs.front(); //fetches task
			Jobs.pop_front();

			//Generates the meshes
			
			Chunk* c = server->getChunk(pos);
			
			auto t0 = std::chrono::high_resolution_clock::now();
			ChunkMesher.reset();
			ChunkMesher.setChunk(c);
			ChunkMesher.GenerateMesh();

			//Transfer Infomation
			ChunkVertexData* data = new ChunkVertexData();
			data->solidVertices.resize(ChunkMesher.solidFaceCount * 12);
			data->transparentVertices.resize(ChunkMesher.transparentFaceCount * 12);
			data->position_ = pos;

			memcpy(data->solidVertices.data(), ChunkMesher.VerticesBuffer.data(), ChunkMesher.solidFaceCount * 12 * sizeof(uint32_t));
			memcpy(data->transparentVertices.data(), ChunkMesher.TransparentVerticesBuffer.data(), ChunkMesher.transparentFaceCount * 12 * sizeof(uint32_t));

			auto t1 = std::chrono::high_resolution_clock::now();
			amountOfMeshGenerated++;

			FinishedJobs.push_back(data);

			buildTime += (double)(t1 - t0).count() / 1000.0;
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

		timerSleepNotPrecise(1);
	}

	Logger.LogInfo("Mesher", "Shutting down mesh worker: " + std::to_string(WorkerID));
}

void WorldRender::Update() {
	
	int ChunkUpdateLimit = 4000;

	int UpdateAmount = 0;
	for (int WorkerID = 0; WorkerID < WorkerCount; WorkerID++) {

		WorkerLocks[WorkerID].lock();

		while (!WorkerOutput[WorkerID].empty()) {
			if (ChunkUpdateLimit < UpdateAmount) {
				break;
			}
			//mProfiler->CombineCache(WorkerOutput[(uint64_t)WorkerID].front()->profiler);
			RendererV2.AddChunk(WorkerOutput[(uint64_t)WorkerID].front());
			WorkerOutput[WorkerID].pop_front();
			UpdateAmount++;
		}

		WorkerLocks[WorkerID].unlock();
	}

	LoadChunkMultiToRenderer(server->getUpdatedChunks());

	if (UpdateAmount < ChunkUpdateLimit) {
		RendererV2.Defrag(ChunkUpdateLimit - UpdateAmount);
	}

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

void WorldRender::Start(GLFWwindow* window_, InternalServer* server_, PerformanceProfiler* pProfilerIn) {
	stop = false;

	HorizontalRenderDistance = AppOptions.HorizontalRenderDistance;
	VerticalRenderDistance = AppOptions.VerticalRenderDistance;

	int ThreadCount = AppOptions.MeshThreads;

	window = window_;
	server = server_;
	WorkerCount = ThreadCount;

	RendererV2.Initialize(window, player.getCamera());
	RendererV2.LoadAssets();
	RendererV2.setSettings(HorizontalRenderDistance, VerticalRenderDistance, 90);

	WorkerCount = ThreadCount;

	Workers.resize(ThreadCount);
	WorkerTask.resize(ThreadCount);
	WorkerOutput.resize(ThreadCount);
	WorkerLocks.resize(ThreadCount);

	for (int i = 0; i < ThreadCount; i++) {
		Workers[i] = std::thread(&WorldRender::Worker, this, i);
	}
	Scheduler = std::thread(&WorldRender::TaskScheduler, this);

	mProfiler = pProfilerIn;
}

void WorldRender::TaskScheduler() {

	int WorkerSelection = 0;

	std::deque<std::deque<ChunkPos>> DistributedTasks;

	DistributedTasks.resize(WorkerCount);

	std::deque<ChunkPos> InternalTaskList;

	while (!stop) {

		//Fetch tasks

		SchedulerLock.lock();
		InternalTaskList.insert(InternalTaskList.end(), TaskList.begin(), TaskList.end());
		TaskList.clear();
		SchedulerLock.unlock();

		//Interally distributes the jobs

		while (!InternalTaskList.empty()) {

			ChunkPos task = InternalTaskList.front();
			InternalTaskList.pop_front();

			DistributedTasks[WorkerSelection].push_back(task);

			WorkerSelection++;

			if (WorkerSelection == WorkerCount)
				WorkerSelection = 0;
		}

		//Distributes the tasks

		for (int i = 0; i < WorkerCount; i++) {
			WorkerLocks[i].lock();
			WorkerTask[i].insert(WorkerTask[i].end(), DistributedTasks[i].begin(), DistributedTasks[i].end());
			WorkerLocks[i].unlock();

			DistributedTasks[i].clear();
		}




	}

	Logger.LogInfo("Mesher", "Shutting down mesh scheduler");

}