#pragma once
#include "PlayerPOV.h"
#include "../../RenderEngine/ChunkRenderer/ChunkRenderer.h"
#include "../../RenderEngine/ChunkRenderer/Mesh/ChunkMeshing.h"
#include <unordered_map>
#include <concurrent_unordered_map.h>
#include <concurrent_queue.h>
#include <thread>
#include <mutex>
class WorldRender {
public:
	void Render();

	void LoadChunk(Chunk chunk);

	void SetRotation(glm::dvec2 rotation) {
		player.SetRotation(rotation);
	}

	void SetPosition(glm::dvec3 position) {
		player.SetPosition(position);
	}

	void Start(GLFWwindow* window_);

	void Stop();

	void PauseWorker(int WorkerID);

	void UnpauseWorker(int WorkerID);

	void Update();

	void PauseLoader();

	void UnpauseLoader();

private:

	void MeshWorker(int id);

	void LoaderThread(int id);

	ChunkID GenChunkID(int x, int y, int z);

	PlayerPOV player;
	ChunkRenderer Renderer;
	GLFWwindow* window;

	bool stop = false;

	int MeshWorkerCount = 1;

	int WorkersWriting = 0;

	int WorkerSelect = 0;

	std::vector<std::thread> Workers;
	std::thread LoaderWorker;
	std::vector<std::deque<ChunkID>> WorkerMeshQueue;
	std::vector<concurrency::concurrent_queue<Meshing::ChunkMeshData>> WorkerMeshOutput;
	std::vector<bool> WorkerIsWorking;
	std::vector<bool> WorkerPause;

	bool LoaderPause = false;
	bool LoaderIsWorking = false;

	std::mutex mut;

	concurrency::concurrent_unordered_map<ChunkID, Chunk> ChunkCache;
	std::deque<Chunk> ChunkLoadingCache;
};