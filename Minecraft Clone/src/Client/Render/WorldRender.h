#pragma once
#include "PlayerPOV.h"
#include "../../RenderEngine/ChunkRenderer/ChunkRenderer.h"
#include "../../RenderEngine/ChunkRenderer/Mesh/ChunkMeshing.h"
#include <unordered_map>
#include <concurrent_unordered_map.h>
#include <concurrent_queue.h>
#include <thread>
#include <mutex>
#include "../../World/World.h"
class WorldRender {
public:

	void SetRotation(glm::dvec2 rotation) {
		player.SetRotation(rotation);
	}

	void SetPosition(glm::dvec3 position) {
		player.SetPosition(position);
	}

	void Render();

	void LoadChunkToRenderer(ChunkID chunk);
	
	void Start(GLFWwindow* window_, World* world_, int ThreadCount);

	void Stop();

	void Update();

private:

	void Worker(int id);

	void TaskScheduler();

	PlayerPOV player;
	ChunkRenderer Renderer;
	GLFWwindow* window;
	World* world;

	int WorkerCount = NULL;

	bool stop = false;

	std::deque<ChunkID> TaskList; //All tasks will go in here and they will be distributed to all the workers

	std::deque<std::thread> Workers;
	std::deque<std::deque<ChunkID>> WorkerTask;
	std::deque<std::deque<Meshing::ChunkMeshData>> WorkerOutput;
	std::deque<std::mutex> WorkerLocks;

	std::thread Scheduler;
	std::mutex SchedulerLock;
	
	//Debugging stuff
	int count = 0;
};