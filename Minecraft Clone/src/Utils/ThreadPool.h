// Copyright (c) 2025 Voxel-Game Author. All rights reserved.

#pragma once

#include <atomic>
#include <condition_variable>
#include <deque>
#include <functional>
#include <string>
#include <thread>
#include <vector>

/*
 * Takes in a function (the func that the worker runs)
 * Has a task scheduler and an output manager
 * Can be fed in a templated input and it will provide an output
 */

template <class TaskIn, class TaskOut>
class ThreadPool {
 public:
  ThreadPool(size_t threads, std::string type,
             std::function<TaskOut(TaskIn)> taskfunc,
             size_t batchSize = SIZE_MAX);
  ~ThreadPool();

  void SubmitTask(const std::vector<TaskIn>& task);
  void SubmitTask(const TaskIn& task);

  void EditThreadCount(size_t num_threads);

  std::vector<TaskOut> GetOutput();
  size_t GetQueueSize() const noexcept;

 private:
  void Scheduler(std::stop_token stoken);
  void Worker(std::stop_token stoken, int workerId);
  void OutputManager(std::stop_token stoken);
  void RemoveThread();
  void AddThread();

  // Threads
  std::deque<std::jthread> workers_;
  std::jthread scheduler_;
  std::jthread output_manager_;
  std::atomic<size_t> worker_count_;

  // Condition Variables
  std::deque<std::condition_variable> worker_cv_;
  std::condition_variable scheduler_cv_;
  std::condition_variable output_cv_;
  std::atomic<bool> output_ready_;

  // Mutex
  std::deque<std::mutex> worker_locks_;
  std::deque<std::mutex> worker_output_locks_;
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
};

template <class TaskIn, class TaskOut>
inline ThreadPool<TaskIn, TaskOut>::ThreadPool(
    size_t threads, std::string type, std::function<TaskOut(TaskIn)> taskFunc,
    size_t batchSize)
    : workers_{threads},
      scheduler_{std::bind_front(&ThreadPool::Scheduler, this)},
      output_manager_{std::bind_front(&ThreadPool::OutputManager, this)},
      worker_count_{threads},
      worker_cv_{threads},
      output_ready_{false},
      worker_locks_{threads},
      worker_output_locks_{threads},
      type_{type},
      worker_task_{threads},
      worker_output_{threads},
      batch_size_{batchSize},
      task_func_{taskFunc},
      task_queued_{0} {
  for (size_t i = 0; i < threads; ++i) {
    workers_[i] = std::jthread(
        std::bind(&ThreadPool::Worker, this, std::placeholders::_1, i));
  }
}
template <class TaskIn, class TaskOut>
inline ThreadPool<TaskIn, TaskOut>::~ThreadPool() {
  scheduler_.request_stop();
  output_manager_.request_stop();

  for (size_t i = 0; i < worker_count_.load(); ++i) {
    workers_[i].request_stop();
    workers_[i].join();
  }

  worker_cv_.clear();
  worker_locks_.clear();
  worker_output_locks_.clear();
  type_.clear();
  task_list_.clear();
  output_list_.clear();
  worker_task_.clear();
  worker_output_.clear();
}

template <class TaskIn, class TaskOut>
inline void ThreadPool<TaskIn, TaskOut>::Scheduler(std::stop_token stoken) {
  std::vector<TaskIn> internal_task_list;

  while (!stoken.stop_requested()) {
    {
      std::unique_lock<std::mutex> lock{scheduler_lock_};

      scheduler_cv_.wait_for(
          lock, std::chrono::milliseconds(100), [this, &stoken]() {
            return !task_list_.empty() || stoken.stop_requested();
          });

      if (stoken.stop_requested()) break;

      internal_task_list = std::move(task_list_);
      task_list_.clear();
    }

    // Evenly distribute the tasks
    size_t task_per_worker = internal_task_list.size() / worker_count_.load();
    size_t task_per_worker_remainder =
        internal_task_list.size() % worker_count_.load();

    size_t curr_index = 0;

    for (size_t i = 0; !stoken.stop_requested() && i < worker_count_.load();
         ++i) {
      size_t task_count = task_per_worker;
      if (task_per_worker_remainder != 0) {
        task_count += 1;
        --task_per_worker_remainder;
      }

      if (task_count == 0) break;

      std::lock_guard<std::mutex> lock{worker_locks_[i]};
      worker_task_[i].insert(
          worker_task_[i].end(), internal_task_list.begin() + curr_index,
          internal_task_list.begin() + task_count + curr_index);

      curr_index += task_count;

      worker_cv_[i].notify_one();
    }

    internal_task_list.clear();
  }
}

template <class TaskIn, class TaskOut>
inline void ThreadPool<TaskIn, TaskOut>::Worker(std::stop_token stoken,
                                                int workerId) {
  bool working_on_batch = false;
  size_t batch_index = 0;
  std::vector<TaskIn> worker_tasks;

  while (!stoken.stop_requested()) {
    std::vector<TaskOut> output;

    // Collect task first
    if (!working_on_batch) {
      std::unique_lock<std::mutex> lock{worker_locks_[workerId]};
      worker_cv_[workerId].wait_for(
          lock, std::chrono::milliseconds(100), [this, workerId, &stoken]() {
            return stoken.stop_requested() || !worker_task_[workerId].empty();
          });
      if (stoken.stop_requested()) break;
      worker_tasks = std::move(worker_task_[workerId]);
      worker_task_[workerId].clear();
    }
    size_t i = batch_index;
    for (; !stoken.stop_requested() &&
           i < std::min(worker_tasks.size(), batch_index + batch_size_);
         ++i) {
      if (stoken.stop_requested()) {  // Dump remaining task to TaskList; to
                                      // prevent task lost when changing nums of
                                      // threads
        std::unique_lock<std::mutex> lock{scheduler_lock_};
        task_list_.insert(task_list_.end(),
                          std::make_move_iterator(worker_tasks.begin() + i),
                          std::make_move_iterator(worker_tasks.end()));
        break;
      }
      const TaskIn& task = worker_tasks[i];
      TaskOut out = task_func_(task);
      output.push_back(std::move(out));
    }

    if (i == worker_tasks.size()) {
      worker_tasks.clear();
      working_on_batch = false;
      batch_index = 0;
    } else {
      batch_index = i;
      working_on_batch = true;
    }

    {
      std::lock_guard<std::mutex> lock{worker_output_locks_[workerId]};
      worker_output_[workerId].insert(worker_output_[workerId].end(),
                                      std::make_move_iterator(output.begin()),
                                      std::make_move_iterator(output.end()));
    }

    {
      output_ready_.store(true);
      output_cv_.notify_one();
    }
    output.clear();
  }
}

template <class TaskIn, class TaskOut>
inline void ThreadPool<TaskIn, TaskOut>::OutputManager(std::stop_token stoken) {
  while (!stoken.stop_requested()) {
    // Waits for output
    {
      std::unique_lock<std::mutex> lock{output_lock_};
      output_cv_.wait_for(
          lock, std::chrono::milliseconds(100), [this, &stoken]() {
            return output_ready_.load() || stoken.stop_requested();
          });
      if (stoken.stop_requested()) break;
      output_ready_.store(false);
    }

    // Find workers output and manage them

    for (size_t i = 0; !stoken.stop_requested() && i < worker_count_.load();
         ++i) {
      std::vector<TaskOut> worker_out;
      {
        std::lock_guard<std::mutex> lock{worker_output_locks_[i]};
        worker_out = std::move(worker_output_[i]);
        worker_output_[i].clear();
      }
      if (!worker_out.empty()) {
        std::lock_guard<std::mutex> lock{output_lock_};
        output_list_.insert(output_list_.end(),
                            std::make_move_iterator(worker_out.begin()),
                            std::make_move_iterator(worker_out.end()));
      }
    }
  }
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
inline void ThreadPool<TaskIn, TaskOut>::EditThreadCount(size_t num_threads) {
  if (num_threads == worker_count_.load()) return;
  if (num_threads > worker_count_.load()) {
    size_t to_add = num_threads - worker_count_.load();
    for (int i = 0; i < to_add; ++i) {
      AddThread();
    }
  } else {
    size_t to_remove = worker_count_.load() - num_threads;
    for (int i = 0; i < to_remove; ++i) {
      RemoveThread();
    }
  }
}

template <class TaskIn, class TaskOut>
inline void ThreadPool<TaskIn, TaskOut>::RemoveThread() {
  if (worker_count_.load() == 0) {
    throw std::runtime_error("Tried to remove thread when thread count is 0");
  }
  worker_count_.store(worker_count_.load() - 1);

  // Attempts to terminate thread first
  workers_.back().request_stop();
  workers_.back().join();

  // Now manage the resources of the terminated thread
  {
    std::unique_lock<std::mutex> lock{output_lock_};
    output_list_.insert(output_list_.back(),
                        std::make_move_iterator(worker_output_.back().begin()),
                        std::make_move_iterator(worker_output_.back().end()));
  }

  worker_locks_.pop_back();
  worker_output_.pop_back();
  worker_task_.pop_back();
  worker_output_.pop_back();
  worker_cv_.pop_back();
}

template <class TaskIn, class TaskOut>
inline void ThreadPool<TaskIn, TaskOut>::AddThread() {
  worker_locks_.emplace_back();
  worker_output_locks_.emplace_back();
  worker_cv_.emplace_back();
  worker_task_.emplace_back();
  worker_output_.emplace_back();

  workers_.emplace_back(std::bind(&ThreadPool::Worker, this,
                                  std::placeholders::_1, worker_count_ + 1));
  worker_count_.store(worker_count_.load() + 1);
}

template <class TaskIn, class TaskOut>
inline std::vector<TaskOut> ThreadPool<TaskIn, TaskOut>::GetOutput() {
  std::lock_guard<std::mutex> lock{output_lock_};
  std::vector<TaskOut> out = std::move(output_list_);
  task_queued_ -= out.size();
  return out;
}

template <class TaskIn, class TaskOut>
inline size_t ThreadPool<TaskIn, TaskOut>::GetQueueSize() const noexcept {
  return task_queued_.load();
}
