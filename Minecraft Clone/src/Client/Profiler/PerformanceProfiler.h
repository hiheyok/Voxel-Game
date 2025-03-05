#pragma once
#include <string>
#include <iostream>
#include <stack>

#include "../../Level/Timer/Timer.h"
#include "../../FileManager/Files.h"
#include "../../Level/Typenames.h"



class PerformanceProfiler {
public:
	PerformanceProfiler();

	void ProfileStart(uint64_t hash);

	void ProfileStart(std::string path);

	void ProfileStop(uint64_t hash);

	void ProfileStop(std::string path);

	// TOOD: Fix me
	void RegisterPaths(std::string path);

	uint64_t Hasher(std::string path);

	void LoadCache();

	void print();

	void CondenseCache();

	void CombineCache(PerformanceProfiler profiler);

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