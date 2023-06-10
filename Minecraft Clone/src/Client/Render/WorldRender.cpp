#include "WorldRender.h"
#include "../../Utils/Clock.h"


using namespace glm;

void WorldRender::Render() {
	Renderer.draw();
}

void WorldRender::LoadChunk(Chunk chunk) {
	if (!chunk.isEmpty) {
		ChunkLoadingCache.push_back(chunk);
	}
	
}

ChunkID WorldRender::GenChunkID(int x, int y, int z) {
	long long unsigned int sx = (((1u << 1) - 1u) & (x >> 31));
	long long unsigned int sy = (((1u << 1) - 1u) & (y >> 31));
	long long unsigned int sz = (((1u << 1) - 1u) & (z >> 31));
	long long unsigned int ax = (long long unsigned int)abs(x);
	long long unsigned int ay = (long long unsigned int)abs(y);
	long long unsigned int az = (long long unsigned int)abs(z);

	return 0LLU | ax << 0 | ay << 16 | az << 32 | sx << 61 | sy << 62 | sz << 63;
}

void WorldRender::PauseWorker(int WorkerID) {
	WorkerPause[WorkerID] = true;
	while (WorkerIsWorking[WorkerID]) {}
}

void WorldRender::UnpauseWorker(int WorkerID) {
	WorkerPause[WorkerID] = false;
}

void WorldRender::MeshWorker(int id) {

	int WorkerID = id;

	while (!stop) {

		if (!WorkerPause[WorkerID]) {
			while (WorkerMeshQueue[WorkerID].size() != 0) {

				WorkerIsWorking[WorkerID] = true;

				ChunkID chunk = WorkerMeshQueue[WorkerID].back();
				WorkerMeshQueue[WorkerID].pop_back();

				Chunk* main = new Chunk;
				*main = ChunkCache[chunk];

				Chunk Neighbors[6]{};
				if (ChunkCache.count(GenChunkID(main->Position.x - 1, main->Position.y, main->Position.z))) {
					Neighbors[NX] = ChunkCache[GenChunkID(main->Position.x - 1, main->Position.y, main->Position.z)];
					main->SetNeighbor((ChunkContainer*)&Neighbors[NX], NX);
				}
				if (ChunkCache.count(GenChunkID(main->Position.x + 1, main->Position.y, main->Position.z))) {
					Neighbors[PX] = ChunkCache[GenChunkID(main->Position.x + 1, main->Position.y, main->Position.z)];
					main->SetNeighbor((ChunkContainer*)&Neighbors[PX], PX);
				}
				if (ChunkCache.count(GenChunkID(main->Position.x, main->Position.y - 1, main->Position.z))) {
					Neighbors[NY] = ChunkCache[GenChunkID(main->Position.x, main->Position.y - 1, main->Position.z)];
					main->SetNeighbor((ChunkContainer*)&Neighbors[NY], NY);
				}
				if (ChunkCache.count(GenChunkID(main->Position.x, main->Position.y + 1, main->Position.z))) {
					Neighbors[PY] = ChunkCache[GenChunkID(main->Position.x, main->Position.y + 1, main->Position.z)];
					main->SetNeighbor((ChunkContainer*)&Neighbors[PY], PY);
				}
				if (ChunkCache.count(GenChunkID(main->Position.x, main->Position.y, main->Position.z - 1))) {
					Neighbors[NZ] = ChunkCache[GenChunkID(main->Position.x, main->Position.y, main->Position.z - 1)];
					main->SetNeighbor((ChunkContainer*)&Neighbors[NZ], NZ);
				}
				if (ChunkCache.count(GenChunkID(main->Position.x, main->Position.y, main->Position.z + 1))) {
					Neighbors[PZ] = ChunkCache[GenChunkID(main->Position.x, main->Position.y, main->Position.z + 1)];
					main->SetNeighbor((ChunkContainer*)&Neighbors[PZ], PZ);
				}

				Meshing::ChunkMeshData mesh;
				mesh.GenerateMesh(ChunkCache[chunk]);
				
				WorkerMeshOutput[WorkerID].push(mesh);
				WorkerIsWorking[WorkerID] = false;

				delete main;

				if (WorkerPause[WorkerID])
					break;
			}
		}
		
		timerSleepNotPrecise(5);
	}
}

void WorldRender::Update() {

	for (int WorkerID = 0; WorkerID < MeshWorkerCount; WorkerID++) {
		
		if (!WorkerMeshOutput[WorkerID].empty()) {

			PauseWorker(WorkerID);

			while (!WorkerMeshOutput[WorkerID].empty()) {
				Meshing::ChunkMeshData mesh;
				if (WorkerMeshOutput[WorkerID].try_pop(mesh)) {
					Renderer.AddChunkMesh(mesh);
				}

				
			}

			UnpauseWorker(WorkerID);
			
		}

		UnpauseWorker(WorkerID);
	}
	
	Renderer.UpdateData();
	Renderer.GenCallDrawCommands();

}

void WorldRender::Stop() {
	stop = true;
}

void WorldRender::Start(GLFWwindow* window_) {
	stop = false;

	window = window_;

	Renderer.init(window, player.getCamera());
	Renderer.ReloadAssets();

	Workers.resize(MeshWorkerCount);
	WorkerMeshQueue.resize(MeshWorkerCount);
	WorkerMeshOutput.resize(MeshWorkerCount);
	WorkerIsWorking.resize(MeshWorkerCount);
	WorkerPause.resize(MeshWorkerCount);

	for (int i = 0; i < MeshWorkerCount; i++) {
		Workers[i] = std::thread(&WorldRender::MeshWorker, this, i);
	}
	LoaderWorker = std::thread(&WorldRender::LoaderThread, this, 1);
}

void WorldRender::LoaderThread(int id) {
	
	while (!stop) {
		if (!LoaderPause) {
			LoaderIsWorking = true;

			int loops = 0;

			mut.lock();
			while (ChunkLoadingCache.size() != 0) {

				Chunk ChunkToLoad = ChunkLoadingCache.back();
				ChunkLoadingCache.pop_back(); //change

				ChunkCache[ChunkToLoad.chunkID] = ChunkToLoad;

				if ((WorkerSelect % MeshWorkerCount) == 0)
					WorkerSelect = 0;

				WorkerMeshQueue[WorkerSelect].push_back(ChunkToLoad.chunkID);
				WorkerSelect++;

				

				loops++;

				if (loops == 200) {
					loops = 0;
					break;
				}

				if (LoaderPause)
					break;
			}

			mut.unlock();

			LoaderIsWorking = false;
		}
		timerSleepNotPrecise(5);
	}
	
}

void WorldRender::PauseLoader() {
	LoaderPause = true;
	while (LoaderIsWorking) {}
}

void WorldRender::UnpauseLoader() {
	LoaderPause = false;
}