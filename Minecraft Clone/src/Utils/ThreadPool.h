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
             std::function<TaskOut(TaskIn, int)> taskfunc,
             size_t batchSize = SIZE_MAX);
  ~ThreadPool();

  void SubmitTask(const std::vector<TaskIn>& task);
  void SubmitTask(const TaskIn& task);

  void EditThreadCount(size_t num_threads);

  std::vector<TaskOut> GetOutput();
  size_t GetQueueSize() const noexcept;

 private:
  void Scheduler(std::stop_token stoken);
  void Worker(std::stop_token stoken, int worker_id);
  void OutputManager(std::stop_token stoken);
  void RemoveThread();
  void AddThread();

  // Condition Variables
  std::deque<std::condition_variable> worker_cv_;
  std::condition_variable output_cv_;
  std::condition_variable scheduler_cv_;

  // Mutex
  std::deque<std::mutex> worker_locks_;
  std::deque<std::mutex> worker_output_locks_;
  std::mutex scheduler_lock_;
  std::mutex output_lock_;
  std::string type_;
  std::mutex threads_mutex_;

  // Task list
  std::vector<TaskIn> task_list_;  // This stores all of the task to do
  std::vector<TaskOut> output_list_;
  std::vector<std::vector<TaskIn>> worker_task_;
  std::vector<std::vector<TaskOut>> worker_output_;
  size_t batch_size_;

  // Task func
  std::function<TaskOut(TaskIn, int)> task_func_;

  // Task Queued
  std::atomic<size_t> task_queued_;

  // Threads
  std::deque<std::jthread> workers_;
  std::jthread scheduler_;
  std::jthread output_manager_;
  std::atomic<size_t> worker_count_;
};

template <class TaskIn, class TaskOut>
inline ThreadPool<TaskIn, TaskOut>::ThreadPool(
    size_t threads, std::string type,
    std::function<TaskOut(TaskIn, int)> taskFunc, size_t batchSize)
    : worker_cv_{threads},
      worker_locks_{threads},
      worker_output_locks_{threads},
      type_{type},
      worker_task_{threads},
      worker_output_{threads},
      batch_size_{batchSize},
      task_func_{taskFunc},
      task_queued_{0},
      workers_{threads},
      scheduler_{std::bind_front(&ThreadPool::Scheduler, this)},
      output_manager_{std::bind_front(&ThreadPool::OutputManager, this)},
      worker_count_{threads} {
  for (size_t i = 0; i < threads; ++i) {
    workers_[i] = std::jthread(
        std::bind(&ThreadPool::Worker, this, std::placeholders::_1, i));
  }
}
template <class TaskIn, class TaskOut>
inline ThreadPool<TaskIn, TaskOut>::~ThreadPool() {
  // Request stop for all threads first
  scheduler_.request_stop();
  output_manager_.request_stop();
  
  const size_t worker_count = worker_count_.load();
  for (size_t i = 0; i < worker_count; ++i) {
    workers_[i].request_stop();
  }
  
  // Notify all condition variables to wake up waiting threads
  scheduler_cv_.notify_all();
  output_cv_.notify_all();
  for (size_t i = 0; i < worker_count; ++i) {
    worker_cv_[i].notify_all();
  }
  
  // Now join all threads
  scheduler_.join();
  for (size_t i = 0; i < worker_count; ++i) {
    workers_[i].join();
  }
  output_manager_.join();
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
      // No need to clear - moved-from vector is already empty
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
                                                int worker_id) {
  bool working_on_batch = false;
  size_t batch_index = 0;
  std::vector<TaskIn> worker_tasks;

  while (!stoken.stop_requested()) {
    std::vector<TaskOut> output;

    // Collect task first
    if (!working_on_batch) {
      std::unique_lock<std::mutex> lock{worker_locks_[worker_id]};
      worker_cv_[worker_id].wait_for(
          lock, std::chrono::milliseconds(50), [this, worker_id, &stoken]() {
            return stoken.stop_requested() || !worker_task_[worker_id].empty();
          });
      if (stoken.stop_requested()) break;
      worker_tasks = std::move(worker_task_[worker_id]);
      worker_task_[worker_id].clear();
    }
    size_t i = batch_index;
    for (; !stoken.stop_requested() &&
           i < std::min(worker_tasks.size(), batch_index + batch_size_);
         ++i) {
      if (stoken.stop_requested()) {  // Dump remaining task to TaskList; to
                                      // prevent task lost when changing nums of
                                      // threads if stop midway
        std::unique_lock<std::mutex> lock{scheduler_lock_};
        task_list_.insert(task_list_.end(),
                          std::make_move_iterator(worker_tasks.begin() + i),
                          std::make_move_iterator(worker_tasks.end()));
        break;
      }
      // Execute task
      const TaskIn& task = worker_tasks[i];
      TaskOut out = task_func_(task, worker_id);
      output.emplace_back(std::move(out));
      task_queued_.fetch_sub(1, std::memory_order_acq_rel);
    }

    // Handles batching
    if (i == worker_tasks.size()) {
      worker_tasks.clear();
      working_on_batch = false;
      batch_index = 0;
    } else {
      batch_index = i;
      working_on_batch = true;
    }

    {
      std::lock_guard<std::mutex> lock{worker_output_locks_[worker_id]};
      worker_output_[worker_id].insert(worker_output_[worker_id].end(),
                                       std::make_move_iterator(output.begin()),
                                       std::make_move_iterator(output.end()));
    }

    output_cv_.notify_one();
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
          lock, std::chrono::milliseconds(50), [this, &stoken]() {
            return stoken.stop_requested();
          });
      if (stoken.stop_requested()) break;
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
  if (task.size() == 0) return;
  std::lock_guard<std::mutex> lock{scheduler_lock_};
  task_queued_.fetch_add(task.size(), std::memory_order_acq_rel);
  task_list_.insert(task_list_.end(), task.begin(), task.end());
  scheduler_cv_.notify_one();
}

template <class TaskIn, class TaskOut>
inline void ThreadPool<TaskIn, TaskOut>::SubmitTask(const TaskIn& task) {
  std::lock_guard<std::mutex> lock{scheduler_lock_};
  task_list_.push_back(task);
  scheduler_cv_.notify_one();
  task_queued_.fetch_add(1, std::memory_order_acq_rel);
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
  std::vector<TaskIn> pending_tasks;
  std::vector<TaskOut> pending_outputs;
  std::jthread dying_thread;

  {
    std::scoped_lock registry_lock{threads_mutex_};

    if (worker_count_.load(std::memory_order_relaxed) <= 1) {
      throw std::runtime_error("Cannot have less than 1 thread");
    }

    {
      std::lock_guard lock{worker_locks_.back()};
      pending_tasks = std::move(worker_task_.back());
    }

    {
      std::lock_guard lock{worker_output_locks_.back()};
      pending_outputs = std::move(worker_output_.back());
    }

    dying_thread = std::move(workers_.back());
    dying_thread.request_stop();
    worker_cv_.back().notify_one();

    workers_.pop_back();
    worker_cv_.pop_back();
    worker_locks_.pop_back();
    worker_output_locks_.pop_back();
    worker_task_.pop_back();
    worker_output_.pop_back();
  }

  dying_thread.join();

  {
    std::lock_guard lock{output_lock_};
    output_list_.insert(output_list_.end(),
                        std::make_move_iterator(pending_outputs.begin()),
                        std::make_move_iterator(pending_outputs.end()));
  }
  output_cv_.notify_one();

  {
    std::lock_guard lock{scheduler_lock_};
    task_list_.insert(task_list_.end(),
                      std::make_move_iterator(pending_tasks.begin()),
                      std::make_move_iterator(pending_tasks.end()));
  }
  scheduler_cv_.notify_one();
}

template <class TaskIn, class TaskOut>
inline void ThreadPool<TaskIn, TaskOut>::AddThread() {
  worker_locks_.emplace_back();
  worker_output_locks_.emplace_back();
  worker_cv_.emplace_back();
  worker_task_.emplace_back();
  worker_output_.emplace_back();

  workers_.emplace_back(std::bind(&ThreadPool::Worker, this,
                                  std::placeholders::_1, worker_count_.load()));
  worker_count_.fetch_add(1, std::memory_order_acq_rel);
}

template <class TaskIn, class TaskOut>
inline std::vector<TaskOut> ThreadPool<TaskIn, TaskOut>::GetOutput() {
  std::lock_guard<std::mutex> lock{output_lock_};
  std::vector<TaskOut> out = std::move(output_list_);
  return out;
}

template <class TaskIn, class TaskOut>
inline size_t ThreadPool<TaskIn, TaskOut>::GetQueueSize() const noexcept {
  return task_queued_.load();
}
