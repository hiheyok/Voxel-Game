#pragma once
#include <thread>
#include <deque>
#include <vector>
#include <condition_variable>
#include <string>

/*
* Takes in a function ( the func that the worker runs)
* Has a task scheduler and an output manager
* Can be fed in a templated input and it will provide an output
*/
// TODO: Add dynamic thread allocation (should be easy)
template <class TaskIn, class TaskOut, TaskOut (*Task)(const TaskIn&)>
class ThreadPool {
public:
	ThreadPool(int threads, std::string type, size_t batchSize = SIZE_MAX);

	~ThreadPool();

	void Stop();

	void SubmitTask(const std::vector<TaskIn>& task);
	void SubmitTask(const TaskIn& task);

	std::vector<TaskOut> GetOutput();
	size_t batch_size_;

private:
	// Threads
	std::deque<std::thread> workers_;
	std::thread scheduler_;
	std::thread output_manager_;
	size_t worker_count_;

	// Condition Variables
	std::deque<std::condition_variable> worker_cv_;
	std::condition_variable scheduler_cv_;
	std::condition_variable output_cv_;
	bool stop_;
	bool output_ready_;

	// Mutex
	std::deque<std::mutex> workers_locks_;
	std::deque<std::mutex> workers_output_locks_;
	std::mutex scheduler_lock_;
	std::mutex output_lock_;
	std::string type_;

	// Task list
	std::vector<TaskIn> task_list_; // This stores all of the task to do
	std::vector<TaskOut> output_list_;
	std::vector<std::vector<TaskIn>> worker_task_;
	std::vector<std::vector<TaskOut>> worker_output_;

	void Scheduler();
	void Worker(int workerId);
	void OutputManager();
};

template <class TaskIn, class TaskOut, TaskOut(*Task)(const TaskIn&)>
ThreadPool<TaskIn, TaskOut, Task>::ThreadPool(int threads, std::string type, size_t batchSize) :
	workers_{ threads },
	worker_count_{ threads },
	worker_cv_{ threads },
	type_{ type },
	stop_{ false },
	workers_locks_{ threads },
	workers_output_locks_{ threads },
	worker_task_{ threads },
	worker_output_{ threads },
	output_ready_{ false },
	batch_size_{ batchSize } {

	for (int i = 0; i < threads; ++i) {
		workers_[i] = std::thread(&ThreadPool::Worker, this, i);
	}

	scheduler_ = std::thread(&ThreadPool::Scheduler, this);
	output_manager_ = std::thread(&ThreadPool::OutputManager, this);
}

template <class TaskIn, class TaskOut, TaskOut(*Task)(const TaskIn&)>
ThreadPool<TaskIn, TaskOut, Task>::~ThreadPool() {
	if (!stop_)
		Stop();
}

template <class TaskIn, class TaskOut, TaskOut(*Task)(const TaskIn&)>
void ThreadPool<TaskIn, TaskOut, Task>::Scheduler() {
	g_logger.LogDebug(type_ + " Thread Pool", "Started task scheduler");

	std::vector<TaskIn> internalTaskList;

	while (!stop_) {
		{
			std::unique_lock<std::mutex> lock{ scheduler_lock_ };

			scheduler_cv_.wait_for(lock, std::chrono::milliseconds(100),
				[this]() { return !task_list_.empty() || stop_; });

			if (stop_) break;

			internalTaskList = std::move(task_list_);
			task_list_.clear();
		}


		// Evenly distribute the tasks
		int taskPerWorker = internalTaskList.size() / worker_count_;
		int taskPerWorkerRemainder = internalTaskList.size() % worker_count_;

		int currIndex = 0;

		for (int i = 0; !stop_ && i < worker_count_; ++i) {

			int taskCount = taskPerWorker;
			if (taskPerWorkerRemainder != 0) {
				taskCount += 1;
				--taskPerWorkerRemainder;
			}

			if (taskCount == 0) break;

			std::lock_guard<std::mutex> lock{ workers_locks_[i] };
			worker_task_[i].insert(worker_task_[i].end(),
				internalTaskList.begin() + currIndex,
				internalTaskList.begin() + taskCount + currIndex);

			currIndex += taskCount;

			worker_cv_[i].notify_one();
		}

		internalTaskList.clear();
	}

	g_logger.LogDebug(type_ + " Thread Pool", "Shutting down task scheduler");
}

template <class TaskIn, class TaskOut, TaskOut(*Task)(const TaskIn&)>
void ThreadPool<TaskIn, TaskOut, Task>::Worker(int workerId) {
	g_logger.LogDebug(type_ + " Thread Pool", "Started worker " + std::to_string(workerId));
	bool workingOnBatch = false;
	size_t batchIndex = 0;
	std::vector<TaskIn> workerTasks;

	while (!stop_) {
		std::vector<TaskOut> output;

		// Collect task first
		if (!workingOnBatch) {
			std::unique_lock<std::mutex> lock{ workers_locks_[workerId] };
			worker_cv_[workerId].wait_for(lock, std::chrono::milliseconds(100),
				[this, workerId]() {return stop_ || !worker_task_[workerId].empty(); });
			if (stop_) break;
			workerTasks = std::move(worker_task_[workerId]);
			worker_task_[workerId].clear();
		}
		int i = batchIndex;
		for (; !stop_ && i < std::min(workerTasks.size(), batchIndex + batch_size_); ++i) {
			if (stop_) break;
			const TaskIn& task = workerTasks[i];
			TaskOut out = Task(task);
			output.push_back(std::move(out));
		}
		
		if (i == workerTasks.size()) {
			workerTasks.clear();
			workingOnBatch = false;
			batchIndex = 0;
		} else {
			batchIndex = i;
			workingOnBatch = true;
		}

		if (stop_) break;

		{
			std::lock_guard<std::mutex> lock{ workers_output_locks_[workerId] };
			worker_output_[workerId].insert(worker_output_[workerId].end(),
				std::make_move_iterator(output.begin()),
				std::make_move_iterator(output.end()));
		}

		{
			std::lock_guard<std::mutex> outputLock{ output_lock_ };
			output_ready_ = true;
			output_cv_.notify_one();
		}
		output.clear();

	}
	g_logger.LogDebug(type_ + " Thread Pool", "Shutting down worker " + std::to_string(workerId));
}

template <class TaskIn, class TaskOut, TaskOut(*Task)(const TaskIn&)>
void ThreadPool<TaskIn, TaskOut, Task>::OutputManager() {
	g_logger.LogDebug(type_ + " Thread Pool", "Started output manager ");

	while (!stop_) {
		// Waits for output
		{
			std::unique_lock<std::mutex> lock{ output_lock_ };
			output_cv_.wait_for(lock, std::chrono::milliseconds(100),
				[this]() {return output_ready_ || stop_;  });
			if (stop_) break;
			output_ready_ = false;
		}

		// Find workers output and manage them

		for (int i = 0; !stop_ && i < worker_count_; ++i) {
			std::vector<TaskOut> workerOut;
			{
				std::lock_guard<std::mutex> lock{ workers_output_locks_[i] };
				workerOut = std::move(worker_output_[i]);
				worker_output_[i].clear();
			}
			if (!workerOut.empty()) {
				std::lock_guard<std::mutex> lock{ output_lock_ };
				output_list_.insert(output_list_.end(),
					std::make_move_iterator(workerOut.begin()),
					std::make_move_iterator(workerOut.end()));
			}
		}
	}
	g_logger.LogDebug(type_ + " Thread Pool", "Shutting down output manager ");
}

template <class TaskIn, class TaskOut, TaskOut(*Task)(const TaskIn&)>
void ThreadPool<TaskIn, TaskOut, Task>::SubmitTask(const std::vector<TaskIn>& task) {
	std::lock_guard<std::mutex> lock{ scheduler_lock_ };
	task_list_.insert(task_list_.end(), task.begin(), task.end());
	scheduler_cv_.notify_one();
}

template <class TaskIn, class TaskOut, TaskOut(*Task)(const TaskIn&)>
void ThreadPool<TaskIn, TaskOut, Task>::SubmitTask(const TaskIn& task) {
	std::lock_guard<std::mutex> lock{ scheduler_lock_ };
	task_list_.push_back(task);
	scheduler_cv_.notify_one();
}

template <class TaskIn, class TaskOut, TaskOut(*Task)(const TaskIn&)>
std::vector<TaskOut> ThreadPool<TaskIn, TaskOut, Task>::GetOutput() {
	std::lock_guard<std::mutex> lock{ output_lock_ };
	std::vector<TaskOut> out = std::move(output_list_);
	return out;
}

template <class TaskIn, class TaskOut, TaskOut(*Task)(const TaskIn&)>
void ThreadPool<TaskIn, TaskOut, Task>::Stop() {
	stop_ = true;
	scheduler_.join();
	output_manager_.join();

	for (int i = 0; i < worker_count_; ++i) {
		workers_[i].join();
	}
	worker_cv_.clear();
	workers_locks_.clear();
	workers_output_locks_.clear();
	type_.clear();
	task_list_.clear();
	output_list_.clear();
	worker_task_.clear();
	worker_output_.clear();
}