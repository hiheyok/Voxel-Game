#include "Generator.h"
#include "../Chunk/ChunkPos/ChunkPos.h"
#include "../../Utils/Clock.h"
#include "Generators/GeneratorType.h"
#include <concurrent_unordered_set.h>

using namespace std;
using namespace glm;

vector<Chunk*> ChunkGeneration::GetOutput() {
	std::lock_guard<std::mutex> lock{ scheduler_lock_ };
	vector<Chunk*> out = std::move(output_);
	return out;
}

void ChunkGeneration::Start(int threadCount, long long int worldSeedIn) {
	world_seed_ = worldSeedIn;
	WorldGenerator::SetSeed(worldSeedIn);

	worker_count_ = threadCount;

	workers_.resize(threadCount);
	worker_task_.resize(threadCount);
	worker_output_.resize(threadCount);
	worker_locks_.resize(threadCount);

	stop = false;

	for (int i = 0; i < threadCount; i++) {
		workers_[i] = thread(&ChunkGeneration::Worker, this, i);
	}

	scheduler_ = thread(&ChunkGeneration::TaskScheduler, this);

}

void ChunkGeneration::Stop() {
	stop = true;
	scheduler_.join();
	for (size_t i = 0; i < workers_.size(); i++) {
		workers_[i].join();
	}
}

void ChunkGeneration::Worker(int id) {
	const int workerId = id;

	deque<std::pair<ChunkPos, WorldGeneratorID>> jobs;

	deque<Chunk*> finishedJobs;

	while (!stop) {
		WorldGeneratorID generatorUse = g_generators.DEBUG;
		//Fetches all of the tasks and put it in "Jobs"
		{
			std::lock_guard<std::mutex> lock{ worker_locks_[workerId] };
			jobs.insert(jobs.end(), std::make_move_iterator(worker_task_[workerId].begin()), std::make_move_iterator(worker_task_[workerId].end()));
			worker_task_[workerId].clear();
		}
		

		//Generates the chunks

		const int numJobs = jobs.size();
		const int batchSize = 100;
		
		for (int i = 0; i < std::min(batchSize, numJobs); i++) {
			std::pair<ChunkPos, WorldGeneratorID> task = std::move(jobs.front()); //fetches task
			jobs.pop_front();
			//Generate
			const ChunkPos& pos = task.first;
			generatorUse = task.second;

			if (!g_generators.GetGenerator(generatorUse)->use_tall_chunks_) {
				Chunk* chunk = g_generators.GetGenerator(generatorUse)->Generate(pos);

				chunk->position_ = pos;
				finishedJobs.push_back(chunk);
			}
			else {
				if (pos.y != 0) {
					TallChunk* chunk = new TallChunk();
					std::vector<Chunk*> chunks = chunk->GetCubicChunks();

					finishedJobs.insert(finishedJobs.end(), std::make_move_iterator(chunks.begin()), std::make_move_iterator(chunks.end()));
					continue;
				}

				TallChunk* chunk = g_generators.GetGenerator(generatorUse)->GenerateTall(pos);
				//Decompose
				std::vector<Chunk*> chunks = chunk->GetCubicChunks();

				finishedJobs.insert(finishedJobs.end(), std::make_move_iterator(chunks.begin()), std::make_move_iterator(chunks.end()));
			}
		}

		if (finishedJobs.size() != 0) {
			std::lock_guard<std::mutex> lock{ worker_locks_[workerId] };
			worker_output_[workerId].insert(worker_output_[workerId].end(), std::make_move_iterator(finishedJobs.begin()), std::make_move_iterator(finishedJobs.end()));
			finishedJobs.clear();
		}
		
		timerSleepNotPrecise(1);
	}

	jobs.clear();
	Logger.LogInfo("World", "Shutting down world gen worker: " + to_string(workerId));
}

void ChunkGeneration::TaskScheduler() {
	deque<deque<std::pair<ChunkPos, WorldGeneratorID>>> distributedTasks;
	deque<deque<Chunk*>> chunkOutputs;

	distributedTasks.resize(worker_count_);
	chunkOutputs.resize(worker_count_);

	vector<std::pair<ChunkPos, WorldGeneratorID>> internalTaskList;

	while (!stop) {
		
		//Fetch tasks
		{
			std::lock_guard<std::mutex> lock{ scheduler_lock_ };
			internalTaskList = std::move(task_list_);
			task_list_.clear();
		}

		//Interally distributes the jobs

		for (int i = 0; i < internalTaskList.size(); i++) {
			std::pair<ChunkPos, WorldGeneratorID> task = std::move(internalTaskList[i]);

			distributedTasks[i % worker_count_].push_back(std::move(task));
		}
		internalTaskList.clear();

		//Distributes the tasks

		for (int i = 0; i < worker_count_; i++) {
			{
				std::lock_guard<std::mutex> lock{ worker_locks_[i] };
				worker_task_[i].insert(worker_task_[i].end(), std::make_move_iterator(distributedTasks[i].begin()),std::make_move_iterator(distributedTasks[i].end()));
			}
			distributedTasks[i].clear();
		}

		//Fetches worker output
		for (int i = 0; i < worker_count_; i++) {
			std::lock_guard<std::mutex> lock{ worker_locks_[i] };
			chunkOutputs[i].insert(chunkOutputs[i].end(), std::make_move_iterator(worker_output_[i].begin()), std::make_move_iterator(worker_output_[i].end()));
			worker_output_[i].clear();
		}

		//Output the chunks so it can be used
		{
			std::lock_guard<std::mutex> lock{ scheduler_lock_ };
			for (int i = 0; i < worker_count_; i++) {
				output_.insert(output_.end(), std::make_move_iterator(chunkOutputs[i].begin()), std::make_move_iterator(chunkOutputs[i].end()));
				chunkOutputs[i].clear();
			}
		}
		
		
		timerSleepNotPrecise(1);

	}

	Logger.LogInfo("World", "Shutting down world gen scheduler");
}

void ChunkGeneration::Generate(ChunkPos pos, WorldGeneratorID genTypeIn) {
	std::lock_guard<std::mutex> lock{ scheduler_lock_ };
	task_list_.emplace_back(std::pair<ChunkPos, WorldGeneratorID>(pos, genTypeIn));
}

void ChunkGeneration::Generate(vector<ChunkPos> ids, WorldGeneratorID genTypeIn) {
	std::lock_guard<std::mutex> lock{ scheduler_lock_ };
	for (const auto& chunkId : ids) {
		task_list_.emplace_back(std::pair<ChunkPos, WorldGeneratorID>(chunkId, genTypeIn));
	}
}