#pragma once
#include <string>
#include <iostream>
#include <stack>

#include "../../Level/Timer/Timer.h"
#include "../../Utils/Containers/skaHashmap.h"
#include "../../FileManager/Files.h"



class PerformanceProfiler {
public:
	PerformanceProfiler() {
		initial_time_ = std::chrono::high_resolution_clock::now();
	}

	void ProfileStart(uint64_t hash) {
		timer_stack_.emplace(hash, 0);
		timer_stack_.top().second = (std::chrono::high_resolution_clock::now() - initial_time_).count();
	}

	void ProfileStart(std::string path) {
		uint64_t hash = Hasher(path);
		if (!string_hash_container_.count(hash)) {
			RegisterPaths(path);
		}

		timer_stack_.emplace(hash, (std::chrono::high_resolution_clock::now() - initial_time_).count());
	}

	void ProfileStop(uint64_t hash) {
		uint64_t timePass = (std::chrono::high_resolution_clock::now() - initial_time_).count() - timer_stack_.top().second;
		time_pass_cache_.emplace_back(hash, timePass);
		timer_stack_.pop();
	}

	void ProfileStop(std::string path) {
		uint64_t hash = Hasher(path);
		uint64_t timePass = (std::chrono::high_resolution_clock::now() - initial_time_).count() - timer_stack_.top().second;

		time_pass_cache_.emplace_back(hash, timePass);
		timer_stack_.pop();
	}

	// TOOD: Fix me
	void RegisterPaths(std::string path) {
		string_hash_container_[Hasher(path)] = path;
		std::vector<std::string> tokens = Tokenize(path, '/');
		std::vector<std::string> pathTokens = {};

		if (tokens.size() != 1) {
			pathTokens.insert(pathTokens.end(), tokens.begin(), tokens.end());
		}

		string_tokenized_hash_container_[Hasher(path)] = pathTokens;
		
	}

	uint64_t Hasher(std::string path) {
		return std::hash<std::string>{}(path);
	}

	void LoadCache() {
		CondenseCache();

		for (const auto& [hash, time] : time_pass_cache_) {
			std::vector<std::string> tokens = string_tokenized_hash_container_[hash];
			root_.ChangeTime(tokens, 0, (double)time);
		}

		time_pass_cache_.clear();
	}

	void print() {
		root_.print();
	}

	void CondenseCache() {
		FastHashMap<uint64_t, uint64_t> condensedCache = {};

		for (const auto& [hash, time] : time_pass_cache_) {
			if (condensedCache.count(hash)) {
				condensedCache[hash] += time;
			}
			else {
				condensedCache[hash] = time;
			}
		}

		time_pass_cache_.clear();

		time_pass_cache_.insert(time_pass_cache_.end(), condensedCache.begin(), condensedCache.end());

	}

	void CombineCache(PerformanceProfiler profiler) {
		//Simplify cache first
		profiler.CondenseCache();
		time_pass_cache_.insert(time_pass_cache_.end(), profiler.time_pass_cache_.begin(), profiler.time_pass_cache_.end());
		string_hash_container_.insert(profiler.string_hash_container_.begin(), profiler.string_hash_container_.end());
		string_tokenized_hash_container_.insert(profiler.string_tokenized_hash_container_.begin(), profiler.string_tokenized_hash_container_.end());
		CondenseCache();
	}

	std::vector<std::pair<uint64_t, uint64_t>> time_pass_cache_{};

	FastHashMap<uint64_t, std::string> string_hash_container_;
	FastHashMap<uint64_t, std::vector<std::string>> string_tokenized_hash_container_;
private:
	std::stack<std::pair<uint64_t, uint64_t>> timer_stack_;
	

	struct PerformanceTree {
		PerformanceTree() {}
		PerformanceTree(std::string name) : name_(name) {};
		
		std::string name_ = "";
		double time_passed_ = 0.0;
		std::vector<PerformanceTree*> nodes_{};

		void ChangeTime(std::vector<std::string>& path, int depth, double time) {
			time_passed_ += time;

			if (path.size() == depth) {
				return;
			}

			for (PerformanceTree* node : nodes_) {
				if (!strcmp(node->name_.c_str(), path[depth].c_str())) {
					node->ChangeTime(path, depth + 1, time);
					return;
				}
			}

			nodes_.push_back(new PerformanceTree(path[depth]));
			nodes_.back()->ChangeTime(path, depth + 1, time);
		}
		
		void print(int depth = 0) {
			std::string out = "";

			for (int i = 0; i < depth; i++) {
				out += ' ';
			}

			if (depth != 0) {
				out += '-';
			}

			out += name_ + ':';
			out += std::to_string(time_passed_ / 1000000.0) + " ms\n";
			std::cout << out;

			for (PerformanceTree* node : nodes_) {
				node->print(depth + 1);
			}
		}
		
	};

	PerformanceTree root_{"root"};
	std::chrono::steady_clock::time_point initial_time_;

};