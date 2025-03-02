#include "WorldRender.h"
#include "../../Utils/Clock.h"
#include "../../Core/Options/Option.h"

using namespace glm;
using namespace MeshingV2;

void WorldRender::SetRotation(dvec2 rotation_) {
	player_.SetRotation(rotation_);
}

void WorldRender::SetPosition(dvec3 position) {
	player_.SetPosition(position);
}

void WorldRender::Render() {
	renderer_v2_.RenderSky();
	renderer_v2_.Render();
}

void WorldRender::LoadChunkToRenderer(ChunkPos chunk) {
	if (server_->GetChunk(chunk) != nullptr) {
		std::lock_guard<std::mutex> lock{ scheduler_lock_ };
		task_list_.push_back(chunk);
	}

}

void WorldRender::LoadChunkMultiToRenderer(std::vector<ChunkPos> chunks) {
	std::lock_guard<std::mutex> lock{ scheduler_lock_ };
	task_list_.insert(task_list_.end(), std::make_move_iterator(chunks.begin()), std::make_move_iterator(chunks.end()));
}


void WorldRender::Worker(int id) {

	const int batchSize = 500;
	const int workerId = id;

	std::deque<ChunkPos> Jobs;
	ChunkMeshData chunkMesher;

	std::deque<std::unique_ptr<ChunkVertexData>> finishedJobs;

	while (!stop) {
		//Fetches all of the tasks and put it in "Jobs"
		{
			std::lock_guard<std::mutex> lock{ worker_locks_[workerId] };
			Jobs.insert(Jobs.end(), std::make_move_iterator(worker_task_[workerId].begin()), std::make_move_iterator(worker_task_[workerId].end()));
			worker_task_[workerId].clear();
		}

		int count = 0;

		while (!Jobs.empty()) {
			ChunkPos pos = Jobs.front(); //fetches task
			Jobs.pop_front();

			//Generates the meshes
			
			Chunk* chunk = server_->GetChunk(pos);

			Timer time = Timer();

			chunkMesher.Reset();
			chunkMesher.SetChunk(chunk);
			chunkMesher.GenerateMesh();

			//Transfer Infomation
			std::unique_ptr<ChunkVertexData> data = std::make_unique<ChunkVertexData>();
			data->solidVertices.resize(chunkMesher.solid_face_count_ * 12);
			data->transparentVertices.resize(chunkMesher.transparent_face_count_ * 12);
			data->position_ = pos;

			memcpy(data->solidVertices.data(), chunkMesher.vertices_buffer_.data(), chunkMesher.solid_face_count_ * 12 * sizeof(uint32_t));
			memcpy(data->transparentVertices.data(), chunkMesher.transparent_vertices_buffer_.data(), chunkMesher.transparent_face_count_ * 12 * sizeof(uint32_t));

			amountOfMeshGenerated++;

			finishedJobs.push_back(std::move(data));

			buildTime += time.GetTimePassed_μs();
			
			if (++count == batchSize) {
				break;
			}
		}

		if (finishedJobs.size() != 0) {
			std::lock_guard<std::mutex> lock{ worker_locks_[workerId] };
			worker_output_[workerId].insert(worker_output_[workerId].end(), std::make_move_iterator(finishedJobs.begin()), std::make_move_iterator(finishedJobs.end()));
			finishedJobs.clear();
		}

		timerSleepNotPrecise(1);
	}

	Logger.LogInfo("Mesher", "Shutting down mesh worker: " + std::to_string(workerId));
}

void WorldRender::Update() {
	
	const int chunkUpdateLimit = 4000;

	int updateAmount = 0;
	for (size_t workerId = 0; workerId < worker_count_; workerId++) {
		std::lock_guard<std::mutex> lock{ worker_locks_[workerId] };

		while (!worker_output_[workerId].empty()) {
			if (chunkUpdateLimit == updateAmount) {
				break;
			}
			//profiler_->CombineCache(worker_output_[(uint64_t)workerId].front()->profiler);
			renderer_v2_.AddChunk(std::move(worker_output_[workerId].front()));
			worker_output_[workerId].pop_front();
		}

	}

	LoadChunkMultiToRenderer(server_->GetUpdatedChunks());

	if (updateAmount < chunkUpdateLimit) {
		renderer_v2_.Defrag(chunkUpdateLimit - updateAmount);
	}

	renderer_v2_.Update();
	renderer_v2_.PrepareRenderer();

}

void WorldRender::Stop() {
	stop = true;
	Scheduler.join();
	for (int i = 0; i < workers_.size(); i++) {
		workers_[i].join();
	}

	renderer_v2_.Cleanup();

}

void WorldRender::Start(GLFWwindow* window, InternalServer* server, PerformanceProfiler* profiler) {
	stop = false;

	horizontal_render_distance_ = AppOptions.horizontal_render_distance_;
	vertical_render_distance_ = AppOptions.vertical_render_distance_;

	int threadCount = AppOptions.MeshThreads;

	window_ = window;
	server_ = server;
	worker_count_ = threadCount;

	renderer_v2_.Initialize(window_, player_.getCamera());
	renderer_v2_.LoadAssets();
	renderer_v2_.setSettings(horizontal_render_distance_, vertical_render_distance_, 90);

	workers_.resize(threadCount);
	worker_task_.resize(threadCount);
	worker_output_.resize(threadCount);
	worker_locks_.resize(threadCount);

	for (int i = 0; i < threadCount; i++) {
		workers_[i] = std::thread(&WorldRender::Worker, this, i);
	}

	Scheduler = std::thread(&WorldRender::TaskScheduler, this);
	profiler_ = profiler;
}

void WorldRender::TaskScheduler() {

	int workerSelection = 0;

	std::deque<std::deque<ChunkPos>> distributedTasks;

	distributedTasks.resize(worker_count_);

	std::deque<ChunkPos> internalTaskList;

	while (!stop) {

		//Fetch tasks
		{
			std::lock_guard<std::mutex> lock{ scheduler_lock_ };
			internalTaskList.insert(internalTaskList.end(), std::make_move_iterator(task_list_.begin()), std::make_move_iterator(task_list_.end()));
			task_list_.clear();
		}

		//Interally distributes the jobs

		while (!internalTaskList.empty()) {

			ChunkPos task = std::move(internalTaskList.front());
			internalTaskList.pop_front();

			distributedTasks[workerSelection].push_back(std::move(task));

			workerSelection++;

			if (workerSelection == worker_count_)
				workerSelection = 0;
		}

		//Distributes the tasks

		for (int i = 0; i < worker_count_; i++) {
			{
				std::lock_guard<std::mutex> lock{ worker_locks_[i] };
				worker_task_[i].insert(worker_task_[i].end(), std::make_move_iterator(distributedTasks[i].begin()), std::make_move_iterator(distributedTasks[i].end()));
			}
			
			distributedTasks[i].clear();
		}
	}
	Logger.LogInfo("Mesher", "Shutting down mesh scheduler");
}