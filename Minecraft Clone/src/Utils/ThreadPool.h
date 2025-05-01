#pragma once
#include <atomic>
#include <condition_variable>
#include <deque>
#include <functional>
#include <string>
#include <thread>
#include <vector>

#include "Utils/LogUtils.h"

/*
 * Takes in a function (the func that the worker runs)
 * Has a task scheduler and an output manager
 * Can be fed in a templated input and it will provide an output
 */
// TODO: Add dynamic thread allocation (should be easy)
template <class TaskIn, class TaskOut>
class ThreadPool {
 public:
  ThreadPool(size_t threads, std::string type,
             std::function<TaskOut(TaskIn)> taskfunc,
             size_t batchSize = SIZE_MAX);
  ~ThreadPool();

  void Stop();

  void SubmitTask(const std::vector<TaskIn>& task);
  void SubmitTask(const TaskIn& task);

  std::vector<TaskOut> GetOutput();
  size_t GetQueueSize();

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
  std::atomic<bool> stop_;
  bool output_ready_;

  // Mutex
  std::deque<std::mutex> workers_locks_;
  std::deque<std::mutex> workers_output_locks_;
  std::mutex scheduler_lock_;
  std::mutex output_lock_;
  std::string type_;

  // Task list
  std::vector<TaskIn> task_list_;  // This stores all of the task to do
  std::vector<TaskOut> output_list_;
  std::vector<std::vector<TaskIn>> worker_task_;
  std::vector<std::vector<TaskOut>> worker_output_;
  size_t batch_size_;

  // Task func
  std::function<TaskOut(TaskIn)> task_func_;

  // Task Queued
  std::atomic<size_t> task_queued_;

  void Scheduler();
  void Worker(int workerId);
  void OutputManager();
};

template <class TaskIn, class TaskOut>
inline ThreadPool<TaskIn, TaskOut>::ThreadPool(
    size_t threads, std::string type, std::function<TaskOut(TaskIn)> taskFunc,
    size_t batchSize)
    : workers_{threads},
      scheduler_{&ThreadPool::Scheduler, this},
      output_manager_{&ThreadPool::OutputManager, this},
      worker_count_{threads},
      worker_cv_{threads},
      stop_{false},
      output_ready_{false},
      workers_locks_{threads},
      workers_output_locks_{threads},
      type_{type},
      worker_task_{threads},
      worker_output_{threads},
      batch_size_{batchSize},
      task_func_{taskFunc},
      task_queued_{0} {
  for (size_t i = 0; i < threads; ++i) {
    workers_[i] = std::thread(&ThreadPool::Worker, this, i);
  }
}
template <class TaskIn, class TaskOut>
inline ThreadPool<TaskIn, TaskOut>::~ThreadPool() {
  if (!stop_.load()) {
    Stop();
  }
}

template <class TaskIn, class TaskOut>
inline void ThreadPool<TaskIn, TaskOut>::Scheduler() {
  g_logger.LogDebug("ThreadPool::Scheduler",
                    type_ + " Thread Pool | Started task scheduler");

  std::vector<TaskIn> internalTaskList;

  while (!stop_.load()) {
    {
      std::unique_lock<std::mutex> lock{scheduler_lock_};

      scheduler_cv_.wait_for(lock, std::chrono::milliseconds(100),
                             [this]() { return !task_list_.empty() || stop_; });

      if (stop_) break;

      internalTaskList = std::move(task_list_);
      task_list_.clear();
    }

    // Evenly distribute the tasks
    size_t taskPerWorker = internalTaskList.size() / worker_count_;
    size_t taskPerWorkerRemainder = internalTaskList.size() % worker_count_;

    size_t currIndex = 0;

    for (size_t i = 0; !stop_ && i < worker_count_; ++i) {
      size_t taskCount = taskPerWorker;
      if (taskPerWorkerRemainder != 0) {
        taskCount += 1;
        --taskPerWorkerRemainder;
      }

      if (taskCount == 0) break;

      std::lock_guard<std::mutex> lock{workers_locks_[i]};
      worker_task_[i].insert(worker_task_[i].end(),
                             internalTaskList.begin() + currIndex,
                             internalTaskList.begin() + taskCount + currIndex);

      currIndex += taskCount;

      worker_cv_[i].notify_one();
    }

    internalTaskList.clear();
  }

  g_logger.LogDebug("ThreadPool::Scheduler",
                    type_ + " Thread Pool | Shutting down task scheduler");
}

template <class TaskIn, class TaskOut>
inline void ThreadPool<TaskIn, TaskOut>::Worker(int workerId) {
  g_logger.LogDebug(
      "ThreadPool::Worker",
      type_ + " Thread Pool | Started worker " + std::to_string(workerId));
  bool workingOnBatch = false;
  size_t batchIndex = 0;
  std::vector<TaskIn> workerTasks;

  while (!stop_.load()) {
    std::vector<TaskOut> output;

    // Collect task first
    if (!workingOnBatch) {
      std::unique_lock<std::mutex> lock{workers_locks_[workerId]};
      worker_cv_[workerId].wait_for(
          lock, std::chrono::milliseconds(100), [this, workerId]() {
            return stop_ || !worker_task_[workerId].empty();
          });
      if (stop_) break;
      workerTasks = std::move(worker_task_[workerId]);
      worker_task_[workerId].clear();
    }
    size_t i = batchIndex;
    for (; !stop_ && i < std::min(workerTasks.size(), batchIndex + batch_size_);
         ++i) {
      if (stop_) break;
      const TaskIn& task = workerTasks[i];
      TaskOut out = task_func_(task);
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
      std::lock_guard<std::mutex> lock{workers_output_locks_[workerId]};
      worker_output_[workerId].insert(worker_output_[workerId].end(),
                                      std::make_move_iterator(output.begin()),
                                      std::make_move_iterator(output.end()));
    }

    {
      std::lock_guard<std::mutex> outputLock{output_lock_};
      output_ready_ = true;
      output_cv_.notify_one();
    }
    output.clear();
  }
  g_logger.LogDebug("ThreadPool::Worker",
                    type_ + " Thread Pool | Shutting down worker " +
                        std::to_string(workerId));
}

template <class TaskIn, class TaskOut>
inline void ThreadPool<TaskIn, TaskOut>::OutputManager() {
  g_logger.LogDebug("ThreadPool::Worker",
                    type_ + " Thread Pool | Started output manager");
  while (!stop_.load()) {
    // Waits for output
    {
      std::unique_lock<std::mutex> lock{output_lock_};
      output_cv_.wait_for(lock, std::chrono::milliseconds(100),
                          [this]() { return output_ready_ || stop_; });
      if (stop_) break;
      output_ready_ = false;
    }

    // Find workers output and manage them

    for (size_t i = 0; !stop_ && i < worker_count_; ++i) {
      std::vector<TaskOut> workerOut;
      {
        std::lock_guard<std::mutex> lock{workers_output_locks_[i]};
        workerOut = std::move(worker_output_[i]);
        worker_output_[i].clear();
      }
      if (!workerOut.empty()) {
        std::lock_guard<std::mutex> lock{output_lock_};
        output_list_.insert(output_list_.end(),
                            std::make_move_iterator(workerOut.begin()),
                            std::make_move_iterator(workerOut.end()));
      }
    }
  }
  g_logger.LogDebug("ThreadPool::Worker",
                    type_ + " Thread Pool | Shutting down output manager");
}

template <class TaskIn, class TaskOut>
inline void ThreadPool<TaskIn, TaskOut>::SubmitTask(
    const std::vector<TaskIn>& task) {
  std::lock_guard<std::mutex> lock{scheduler_lock_};
  task_queued_ += task.size();
  task_list_.insert(task_list_.end(), task.begin(), task.end());
  scheduler_cv_.notify_one();
}

template <class TaskIn, class TaskOut>
inline void ThreadPool<TaskIn, TaskOut>::SubmitTask(const TaskIn& task) {
  std::lock_guard<std::mutex> lock{scheduler_lock_};
  task_list_.push_back(task);
  scheduler_cv_.notify_one();
  task_queued_++;
}

template <class TaskIn, class TaskOut>
inline std::vector<TaskOut> ThreadPool<TaskIn, TaskOut>::GetOutput() {
  std::lock_guard<std::mutex> lock{output_lock_};
  std::vector<TaskOut> out = std::move(output_list_);
  task_queued_ -= out.size();
  return out;
}

template <class TaskIn, class TaskOut>
inline void ThreadPool<TaskIn, TaskOut>::Stop() {
  stop_.store(true);
  scheduler_.join();
  output_manager_.join();

  for (size_t i = 0; i < worker_count_; ++i) {
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

template <class TaskIn, class TaskOut>
inline size_t ThreadPool<TaskIn, TaskOut>::GetQueueSize() {
  return task_queued_.load();
}
