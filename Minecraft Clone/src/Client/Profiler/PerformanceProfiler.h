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
		InitialTime = std::chrono::high_resolution_clock::now();
	}

	void ProfileStart(uint64_t hash) {
		TimerStack.emplace(hash, 0);
		TimerStack.top().second = (std::chrono::high_resolution_clock::now() - InitialTime).count();
	}

	void ProfileStart(std::string path) {
		uint64_t hash = hasher(path);
		if (!StringHashContainer.count(hash)) {
			RegisterPaths(path);
		}

		TimerStack.emplace(hash, (std::chrono::high_resolution_clock::now() - InitialTime).count());
	}

	void ProfileStop(uint64_t hash) {
		uint64_t timePass = (std::chrono::high_resolution_clock::now() - InitialTime).count() - TimerStack.top().second;
		TimePassCache.emplace_back(hash, timePass);
		TimerStack.pop();
	}

	void ProfileStop(std::string path) {
		uint64_t hash = hasher(path);
		uint64_t timePass = (std::chrono::high_resolution_clock::now() - InitialTime).count() - TimerStack.top().second;

		TimePassCache.emplace_back(hash, timePass);
		TimerStack.pop();
	}

	void RegisterPaths(std::string path) {
		StringHashContainer[hasher(path)] = path;
		std::vector<std::string> tokens = Tokenize(path, '/');
		std::vector<std::string> pathTokens = {};

		if (tokens.size() != 1) {
			pathTokens.insert(pathTokens.end(), tokens.begin(), tokens.end());
		}

		StringTokenizedHashContainer[hasher(path)] = pathTokens;
		
	}

	uint64_t hasher(std::string path) {
		return std::hash<std::string>{}(path);
	}

	void LoadCache() {
		CondenseCache();

		for (const auto& [hash, time] : TimePassCache) {
			std::vector<std::string> tokens = StringTokenizedHashContainer[hash];


			root.ChangeTime(tokens, 0, (double)time);
		}

		TimePassCache.clear();
	}

	void print() {
		root.print();
	}

	void CondenseCache() {
		ska::flat_hash_map<uint64_t, uint64_t> CondensedCache = {};

		for (const auto& [hash, time] : TimePassCache) {
			if (CondensedCache.count(hash)) {
				CondensedCache[hash] += time;
			}
			else {
				CondensedCache[hash] = time;
			}
		}

		TimePassCache.clear();

		TimePassCache.insert(TimePassCache.end(), CondensedCache.begin(), CondensedCache.end());

	}

	void CombineCache(PerformanceProfiler profiler) {
		//Simplify cache first
		profiler.CondenseCache();
		TimePassCache.insert(TimePassCache.end(), profiler.TimePassCache.begin(), profiler.TimePassCache.end());
		StringHashContainer.insert(profiler.StringHashContainer.begin(), profiler.StringHashContainer.end());
		StringTokenizedHashContainer.insert(profiler.StringTokenizedHashContainer.begin(), profiler.StringTokenizedHashContainer.end());
		CondenseCache();
	}

	std::vector<std::pair<uint64_t, uint64_t>> TimePassCache{};

	ska::flat_hash_map<uint64_t, std::string> StringHashContainer;
	ska::flat_hash_map<uint64_t, std::vector<std::string>> StringTokenizedHashContainer;
private:
	std::stack<std::pair<uint64_t, uint64_t>> TimerStack;
	

	struct PerformanceTree {
		PerformanceTree() {}
		PerformanceTree(std::string pName) : mName(pName) {};
		
		std::string mName = "";
		double mTimePassed = 0.0;
		std::vector<PerformanceTree*> mNodes{};

		void ChangeTime(std::vector<std::string>& path, int depth, double time) {
			mTimePassed += time;

			if (path.size() == depth) {
				return;
			}

			for (PerformanceTree* node : mNodes) {
				if (!strcmp(node->mName.c_str(), path[depth].c_str())) {
					node->ChangeTime(path, depth + 1, time);
					return;
				}
			}

			mNodes.push_back(new PerformanceTree(path[depth]));
			mNodes.back()->ChangeTime(path, depth + 1, time);
		}
		
		void print(int depth = 0) {
			std::string out = "";

			for (int i = 0; i < depth; i++) {
				out += ' ';
			}

			if (depth != 0) {
				out += '-';
			}

			out += mName + ':';
			out += std::to_string(mTimePassed / 1000000.0) + " ms\n";
			std::cout << out;

			for (PerformanceTree* node : mNodes) {
				node->print(depth + 1);
			}
		}
		
	};

	PerformanceTree root{"root"};
	std::chrono::steady_clock::time_point InitialTime;

};